#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <iostream>
#include <utility>

#include "AudioHelper.hpp"
#include "DirtyEffect.hpp"
#include "Enemy.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "Image.hpp"
#include "Label.hpp"
#include "LOG.hpp"
// Turret
#include "TurretTypes.hpp"
#include "Plane.hpp"
// Enemy
#include "EnemyTypes.hpp"
#include "PlayScene.hpp"
#include "Resources.hpp"
#include "Sprite.hpp"
#include "Turret.hpp"
#include "TurretButton.hpp"
#include "ToolButton.hpp"
#include "LOG.hpp"
#include "Collider.hpp"
#include "Point.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);

std::vector<int> turretPrice;
// TODO 5 (2/3): Set the cheat code correctly.
const std::vector<int> PlayScene::code
= { ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_ENTER };

Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
	// TODO 6 (1/2): There's a bug in this file, which crashes the game when you win. Try to find it.
	// TODO 6 (2/2): There's a bug in this file, which doesn't update the player's life correctly when getting the first attack. Try to find it.
	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	lives = 10;
	money = 300;
	SpeedMult = 1;
	turretPrice.push_back(PlugGunTurret::Price);
	turretPrice.push_back(DualGunTurret::Price);
	turretPrice.push_back(GreenTurret::Price);
	turretPrice.push_back(RedTurret::Price);
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(TowerGroup = new Group());
	AddNewObject(EnemyGroup = new Group());
	AddNewObject(BulletGroup = new Group());
	AddNewObject(EffectGroup = new Group());
	// Should support buttons.
	DiceCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ReadEnemyWave();
	mapDistance = CalculateBFSDistance();
	ConstructUI();
	imgTarget1 = new Engine::Image("play/target.png", 0, 0);
	imgTarget1->Visible = false;
	//imgTarget2 = new Engine::Image("play/target.png", 0, 0);
	//imgTarget2->Visible = false;
	preview = nullptr;
	//shovel = nullptr;
	UIGroup->AddNewObject(imgTarget1);
	//UIGroup->AddNewObject(imgTarget2);
	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
	if (!mute)
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    else
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, 0.0);
}
void PlayScene::Terminate() {
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(bgmInstance);
	AudioHelper::StopSample(deathBGMInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	int temp;
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
    {
        temp = SpeedMult;
        SpeedMult = 1;
    }
	// Calculate danger zone.
	std::vector<float> reachEndTimes; // the remaining time for each enemy to reach the end
	for (auto& it : EnemyGroup->GetObjects()) {
		reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
	}
	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	std::sort(reachEndTimes.begin(), reachEndTimes.end());
	float newDeathCountDown = -1;
	int danger = lives;
	for (auto& it : reachEndTimes) {
		if (it <= DangerTime) {
			danger--;
			if (danger <= 0) {
				// Death Countdown
				float pos = DangerTime - it;
				if (it > deathCountDown) {
					// Restart Death Count Down BGM.
					AudioHelper::StopSample(deathBGMInstance);
					if (SpeedMult != 0)
						deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
				}
				float alpha = pos / DangerTime;
				alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
				dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
				newDeathCountDown = it;
				break;
			}
		}
	}
	deathCountDown = newDeathCountDown;
	if (SpeedMult == 0)
    {
        AudioHelper::StopSample(deathBGMInstance);
        //SpeedMult = temp;
    }
	if (deathCountDown == -1 && lives > 0) {
		AudioHelper::StopSample(deathBGMInstance);
		dangerIndicator->Tint.a = 0;
	}
	if (SpeedMult == 0)
		deathCountDown = -1;
	for (int i = 0; i < SpeedMult; i++) {
		IScene::Update(deltaTime);
		// Check if we should create new enemy.
		ticks += deltaTime;
		if (enemyWaveData.empty()) {
			if (EnemyGroup->GetObjects().empty()) {
				// Free resources.
				/*delete TileMapGroup;
				delete GroundEffectGroup;
				delete DebugIndicatorGroup;
				delete TowerGroup;
				delete EnemyGroup;
				delete BulletGroup;
				delete EffectGroup;
				delete UIGroup;
				delete imgTarget;*/
                Engine::GameEngine::GetInstance().ChangeScene("win");
			}
			continue;
		}
		auto current = enemyWaveData.front(); //enemyWaveData is a list< pair<int, float> >
		if (ticks < current.second) //
			continue;
		ticks -= current.second;
		enemyWaveData.pop_front();
		const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
		Enemy* enemy;
		switch (current.first) {
		case 1:
			EnemyGroup->AddNewObject(enemy = new RedNormalEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		// TODO 2 (2/3): You need to modify 'resources/enemy1.txt', or 'resources/enemy2.txt' to spawn the new enemy.
		// The format is "[EnemyId] [TimeDelay] [Repeat]".
		// TODO 2 (3/3): Enable the creation of the new enemy.
		case 2:
            EnemyGroup->AddNewObject(enemy = new BlueNormalEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
        case 3:
            EnemyGroup->AddNewObject(enemy = new GreenNormalEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
        case 4:
            EnemyGroup->AddNewObject(enemy = new MiniBoss1(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
        case 5:
            EnemyGroup->AddNewObject(enemy = new Dice1(DiceCoordinate.x, DiceCoordinate.y));
            DiceCoordinate = SpawnCoordinate;
			break;
        case 6:
            EnemyGroup->AddNewObject(enemy = new Dice2(DiceCoordinate.x, DiceCoordinate.y));
            DiceCoordinate = SpawnCoordinate;
			break;
        case 7:
            EnemyGroup->AddNewObject(enemy = new Dice3(DiceCoordinate.x, DiceCoordinate.y));
            DiceCoordinate = SpawnCoordinate;
			break;
        case 8:
            EnemyGroup->AddNewObject(enemy = new Dice4(DiceCoordinate.x, DiceCoordinate.y));
            DiceCoordinate = SpawnCoordinate;
			break;
        case 9:
            EnemyGroup->AddNewObject(enemy = new Dice5(DiceCoordinate.x, DiceCoordinate.y));
            DiceCoordinate = SpawnCoordinate;
			break;
        case 10:
            EnemyGroup->AddNewObject(enemy = new Dice6(DiceCoordinate.x, DiceCoordinate.y));
            DiceCoordinate = SpawnCoordinate;
			break;
		default:
			continue;
		}
		enemy->UpdatePath(mapDistance);
		// Compensate the time lost.
		enemy->Update(ticks);
	}
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}

	if(freeze)
    {
        if( FreezeTime <= 0 )
        {
            freeze = false;
            FreezeTime = 3.5;
        }
        else
            FreezeTime -= deltaTime * SpeedMult;
    }
}
void PlayScene::Draw() const {
	IScene::Draw();
	if (DebugMode) {
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++) {
			for (int j = 0; j < MapWidth; j++) {
				if (mapDistance[i][j] != -1) {
					// Not elegant nor efficient, but it's quite enough for debugging.
					Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw();
				}
			}
		}
	}
	if(freeze && FreezeTime > 0)
        al_draw_filled_rectangle(0, 0, GetClientSize().x, GetClientSize().y, al_map_rgba(0, 0, 255, 50));
}

void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget1->Visible && preview) {
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	/*if ((button & 1) && !imgTarget2->Visible && shovel) {
		// Cancel shovel.
		UIGroup->RemoveObject(shovel->GetObjectIterator());
		shovel = nullptr;
	}*/
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget1->Visible = false;
	}
	else
    {
        imgTarget1->Visible = true;
        imgTarget1->Position.x = x * BlockSize;
        imgTarget1->Position.y = y * BlockSize;
    }

	/*if (!shovel || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget2->Visible = false;
		return;
	}
	imgTarget2->Visible = true;
	imgTarget2->Position.x = x * BlockSize;
	imgTarget2->Position.y = y * BlockSize;*/
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    if(!preview && mx > 1280 && mx < 1600 && my > 320 && my < 832 )
        UIMoney->Text = std::string("$") + std::to_string(GetMoney());
	if (!imgTarget1->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (button & 1) {
		if (mapState[y][x] != TILE_OCCUPIED) {
			if (!preview)
				return;
			// Check if valid.
			if (!CheckSpaceValid(x, y)) {
				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}
			if( preview->turret_id != 4 && preview->turret_id != 5 )
            {
                // Purchase.
                EarnMoney(-preview->GetPrice());
                // Remove Preview.
                preview->GetObjectIterator()->first = false;
                UIGroup->RemoveObject(preview->GetObjectIterator());
                // Construct real turret.
                preview->Position.x = x * BlockSize + BlockSize / 2;
                preview->Position.y = y * BlockSize + BlockSize / 2;
                preview->Enabled = true;
                preview->Preview = false;
                preview->Tint = al_map_rgba(255, 255, 255, 255);
                TowerGroup->AddNewObject(preview);
                // To keep responding when paused.
                preview->Update(0);
                // Remove Preview.
                preview = nullptr;

                mapState[y][x] = TILE_OCCUPIED;
                OnMouseMove(mx, my);
            }
		}
		else
        {
            if (!preview)
				return;
			// Check if valid.
			if (!CheckSpaceValid(x, y)) {
				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}
            //check if tower should be replaced
            for( auto& it : TowerGroup->GetObjects() )
            {
                Turret* turret = dynamic_cast<Turret*>(it);
                if (mx > turret->Position.x - BlockSize/2 && mx < turret->Position.x + BlockSize/2 && my > turret->Position.y - BlockSize/2 && my < turret->Position.y + BlockSize/2 )
                {
                    if( preview->turret_id == 4 )
                    {
                        EarnMoney(turret->GetPrice() / 2);
                        //turret->GetObjectIterator()->first = true;
                        TowerGroup->RemoveObject(turret->GetObjectIterator());
                        //preview->GetObjectIterator()->first = false;
                        UIGroup->RemoveObject(preview->GetObjectIterator());
                        GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", 1, mx, my));
                        // Remove Preview.
                        preview = nullptr;
                        mapState[y][x] = TILE_FLOOR;
                        break;
                    }
                    else if( preview->turret_id == 5 )
                    {
                        this->money -= preview->GetPrice();
                        TowerGroup->RemoveObject(turret->GetObjectIterator());
                        mapState[y][x] = TILE_FLOOR;
                        UIGroup->RemoveObject(preview->GetObjectIterator());
                        preview = nullptr;
                        switch(turret->turret_id)
                        {
                            case 0: preview = new PlugGunTurret(0, 0); break;
                            case 1: preview = new DualGunTurret(0, 0); break;
                            case 2: preview = new GreenTurret(0, 0); break;
                            case 3: preview = new RedTurret(0, 0); break;
                            case 6: preview = new SpinningTurret(0, 0); break;
                            default: break;
                        }
                        this->money += preview->GetPrice();
                        //UIMoney->Text = std::string("$") + std::to_string(GetMoney());
                        UIGroup->AddNewObject(preview);
                    }
                    else if( preview->turret_id == 0 && turret->turret_id == 0 )
                    {
                        UIGroup->RemoveObject(preview->GetObjectIterator());
                        //remove existing tower
                        TowerGroup->RemoveObject(turret->GetObjectIterator() );
                        // Purchase.
                        EarnMoney(-preview->GetPrice());
                        preview = nullptr;
                        preview = new DualGunTurret(0, 0);
                        UIGroup->AddNewObject(preview);
                        // Remove Preview.
                        preview->GetObjectIterator()->first = false;
                        UIGroup->RemoveObject(preview->GetObjectIterator());
                        // Construct real turret.
                        preview->Position.x = x * BlockSize + BlockSize / 2;
                        preview->Position.y = y * BlockSize + BlockSize / 2;
                        preview->Tint = al_map_rgba(255, 255, 255, 255);
                        TowerGroup->AddNewObject(preview);
                        // Remove Preview.
                        preview = nullptr;
                        break;
                    }
                    else if( preview->turret_id == 2 && turret->turret_id == 2 )
                    {
                        UIGroup->RemoveObject(preview->GetObjectIterator());
                        //remove existing tower
                        TowerGroup->RemoveObject(turret->GetObjectIterator() );
                        // Purchase.
                        EarnMoney(-preview->GetPrice());
                        preview = nullptr;
                        preview = new RedTurret(0, 0);
                        UIGroup->AddNewObject(preview);
                        // Remove Preview.
                        preview->GetObjectIterator()->first = false;
                        UIGroup->RemoveObject(preview->GetObjectIterator());
                        // Construct real turret.
                        preview->Position.x = x * BlockSize + BlockSize / 2;
                        preview->Position.y = y * BlockSize + BlockSize / 2;
                        preview->Tint = al_map_rgba(255, 255, 255, 255);
                        TowerGroup->AddNewObject(preview);
                        // Remove Preview.
                        preview = nullptr;
                        break;
                    }
                }

            }
        }
	}
}

int weapon_id = -1;

void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB) {
		// TODO 5 (1/3): Set Tab as a code to active / de-active the debug mode.
		DebugMode = DebugMode^1;
	}
	else {
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		// TODO 5 (3/3): Check whether the input sequence corresponds to the code. If so, active a plane and earn 10000 money.
        // Active a plane : EffectGroup->AddNewObject(new Plane());
		// Earn money : money += 10000;
		//for( auto it = keyStrokes.begin(); it != keyStrokes.end(); it++ )
            //Engine::LOG() << *it << " ";
        std::list<int> code_cp(code.begin(), code.end());
        if( keyStrokes == code_cp )
        {
            EffectGroup->AddNewObject(new Plane());
            money += 10000;
        }
	}
	if (keyCode == ALLEGRO_KEY_Q) {
		// Hotkey for PlugGunTurret.
		if( ( weapon_id < 3 && money < turretPrice[weapon_id+1] ) || weapon_id == 3 )
        {
            UIMoney->Text = std::string("$") + std::to_string(GetMoney());
            weapon_id = -1;
        }
        else
            weapon_id++;

		UIBtnClicked(weapon_id);
	}
	// TODO 3 (5/5): Make the W key to create the new turret.
	else if (keyCode == ALLEGRO_KEY_D) {
        if( preview && preview->turret_id == 4 )
            UIBtnClicked(-1);
        else
            UIBtnClicked(4);
	}
	else if (keyCode == ALLEGRO_KEY_S) {
        if( preview && preview->turret_id == 5 )
            UIBtnClicked(-1);
        else
            UIBtnClicked(5);
	}
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
	else if (keyCode == ALLEGRO_KEY_M) {
		// Hotkey for mute / unmute.
        if (mute)
            AudioHelper::ChangeSampleVolume(bgmInstance, AudioHelper::BGMVolume);
        else
            AudioHelper::ChangeSampleVolume(bgmInstance, 0.0);
        mute = !mute;
	}
}
void PlayScene::Hit() {
	UILives->Text = std::string("Life ") + std::to_string(--lives);
	if (lives <= 0) {
		Engine::GameEngine::GetInstance().ChangeScene("lose");
	}
}
int PlayScene::GetMoney() const {
	return money;
}
void PlayScene::EarnMoney(int money) {
	this->money += money;
	UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap() {
	std::string filename = std::string("resources/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<bool> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
		switch (c) {
		case '0': mapData.push_back(false); break;
		case '1': mapData.push_back(true); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
			mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			if (num)
				TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			else
				TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
	}
}
void PlayScene::ReadEnemyWave() {
	std::string filename = std::string("resources/enemy") + std::to_string(MapId) + ".txt";
	// Read enemy file.
	float type, wait, repeat;
	enemyWaveData.clear();
	std::ifstream fin(filename);
	while (fin >> type && fin >> wait && fin >> repeat) {
		for (int i = 0; i < repeat; i++)
			enemyWaveData.emplace_back(type, wait);
	}
	fin.close();
}
void PlayScene::ConstructUI() {
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
	// Text
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
	UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life: ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88));
	// Buttons
	ConstructButton(0, "play/turret-6.png", PlugGunTurret::Price);
	// TODO 3 (3/5): Create a button to support constructing the new turret.
	ConstructButton(1, "play/turret-7.png", DualGunTurret::Price);
	ConstructButton(2, "play/turret-1.png", GreenTurret::Price);
	ConstructButton(3, "play/turret-2.png", RedTurret::Price);
	ConstructButton(4, "play/shovel.png", 0);
	ConstructButton(5, "play/move.png", 0);
	ConstructButton(6, "play/turret-spin.png", SpinningTurret::Price);
	ConstructButton(7, "play/snowflake.png", Freeze::Price);

	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::ConstructButton(int id, std::string sprite, int price) {
    if( id >= 0 && id <= 3 )
    {
        TurretButton* btn;
        btn = new TurretButton("play/floor.png", "play/dirt.png",
            Engine::Sprite("play/tower-base.png", 1294 + id * 76, 136, 0, 0, 0, 0),
            Engine::Sprite(sprite, 1294 + id * 76, 136 - 8, 0, 0, 0, 0)
            , 1294 + id * 76, 136, price);
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
        UIGroup->AddNewControlObject(btn);
    }
    else if( id == 6 )
    {
        TurretButton* btn;
        btn = new TurretButton("play/floor.png", "play/dirt.png",
            Engine::Sprite("play/tower-base.png", 1294 + id%4 * 76, 212, 0, 0, 0, 0),
            Engine::Sprite(sprite, 1294 + id%4 * 76, 212, 0, 0, 0, 0)
            , 1294 + id%4 * 76, 212, price);
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
        UIGroup->AddNewControlObject(btn);
    }
    else
    {
        ToolButton* btn;
        btn = new ToolButton("play/floor.png", "play/dirt.png", Engine::Sprite(sprite, 1294 + id%4 * 76, 212, 0, 0, 0, 0), 1294 + id%4 * 76, 212, price );
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
        UIGroup->AddNewControlObject(btn);
    }
	// Reference: Class Member Function Pointer and std::bind.
	//btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
}

void PlayScene::UIBtnClicked(int id) {
    Engine::LOG() << "test";
///preview///
	if (preview) {
		UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
	if (id == 0 && money >= PlugGunTurret::Price)
		preview = new PlugGunTurret(0, 0);
	// TODO 3 (4/5): On the new turret button callback, create the new turret.
	else if( id == 1 && money >= DualGunTurret::Price )
        preview = new DualGunTurret(0, 0);
    else if( id == 2 && money >= GreenTurret::Price )
        preview = new GreenTurret(0, 0);
    else if( id == 3 && money >= RedTurret::Price )
        preview = new RedTurret(0, 0);
    else if( id == 4 )
        preview = new Shovel(0, 0);
    else if( id == 5 && money >= Shifter::Price)
        preview = new Shifter(0, 0);
    else if( id == 6 && money >= SpinningTurret::Price )
        preview = new SpinningTurret(0, 0);
    else if( id == 7 && money >= Freeze::Price )
    {
        freeze = true;
        EarnMoney(-Freeze::Price);
        PlayScene::Draw();
        for( auto& it: EnemyGroup->GetObjects())
        {
            Enemy* enemy = dynamic_cast<Enemy*>(it);
            enemy->Slow(0.05, 3.5);
        }
    }

	if (!preview)
        return;
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	UIGroup->AddNewObject(preview);
	if( id != 4 )
        UIMoney->Text = std::string("$") + std::to_string(GetMoney() - preview->GetPrice());
/*
///shovel///
    if (shovel) {
		UIGroup->RemoveObject(shovel->GetObjectIterator());
        shovel = nullptr;
    }
    if (id == 4)
		shovel = new Shovel("play/shovel.png", 0, 0);
	//else if( id == 1 )
	if (!shovel)
        return;
	shovel->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	shovel->Tint = al_map_rgba(255, 255, 255, 200);
	shovel->Preview = true;
	UIGroup->AddNewObject(shovel);
*/
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return false;
	auto map00 = mapState[y][x];
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = map00;
	if (map[0][0] == -1)
		return false;
	for (auto& it : EnemyGroup->GetObjects()) {
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0) pnt.x = 0;
		if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
		if (pnt.y < 0) pnt.y = 0;
		if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1)
			return false;
	}
	// All enemy have path to exit.
	mapState[y][x] = TILE_OCCUPIED;
	mapDistance = map;
	for (auto& it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
		return map;
	que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
	map[MapHeight - 1][MapWidth - 1] = 0;
	while (!que.empty()) {
		Engine::Point p = que.front();
		que.pop();
        for (auto &c : directions) {
            int x = p.x + c.x;
            int y = p.y + c.y;
            if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight ||
                map[y][x] != -1 || mapState[y][x] != TILE_DIRT) {
                continue;
            } else {
                map[y][x] = map[p.y][p.x] + 1;
                que.push(Engine::Point(x, y));
            }
        }
	}
	return map;
}
