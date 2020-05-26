#include "HitController.h"
#include "Hero.h"
bool HitController::init()
{
	//初始化状态
	registeKeyBoardEvent();

	//把用来播放动画的对象加入其中
	for (int i = 0; i < MAX_IMAGE_SPRITE_NUM; i++)
	{
		Sprite* sprite = Sprite::create("KnightImage.png");
		actionSpriteVec[i] = sprite;
		actionSpriteRun[i] = false;
		GlobalParameter::mapNow->getParent()->addChild(sprite,11);
		sprite->setVisible(false);
	}

	this->schedule(schedule_selector(HitController::hitUpdate), 0.5f);
	this->isHit = false;
	return true;
}

void HitController::registeKeyBoardEvent()
{
	auto keyBoardListener = EventListenerKeyboard::create();
	keyBoardListener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_J://射击键
			this->isHit = true;
			break;
		}
	};
	keyBoardListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_J:
			this->isHit = false;
			break;
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyBoardListener, this);
}

void HitController::hitUpdate(float dt)
{
	if (isHit)
	{
		//从spriteVec中取出一个没有被使用的
		Sprite* actionSprite = nullptr;

		static int index;

		for (index = 0; index < MAX_IMAGE_SPRITE_NUM; index++)
		{
			//当前未播放
			if (actionSpriteRun[index] == false)
			{
				actionSprite = actionSpriteVec[index];
				actionSpriteRun[index] = true;
				break;
			}
		}

		Point hitPosition = GlobalParameter::hero->getPosition();
		//根据当前位置设定初始坐标
		if (GlobalParameter::rightSide)
		{
			if (GlobalParameter::upSide)
			{
				//右上
				hitPosition.x += 45;
				hitPosition.y += 15;
			}
			else if (GlobalParameter::downSide)
			{
				//右下
				hitPosition.x += 55;
				hitPosition.y -= 45;
			}
			else
			{
				//仅朝右
				hitPosition.x += 55;
				hitPosition.y -= 15;
			}
		}
		else if (GlobalParameter::leftSide)
		{
			if (GlobalParameter::upSide)
			{
				//左上
				hitPosition.x -= 45;
				hitPosition.y += 15;
			}
			else if (GlobalParameter::downSide)
			{
				//左下
				hitPosition.x -= 55;
				hitPosition.y -= 45;
			}
			else
			{
				//仅朝左
				hitPosition.x -= 55;
				hitPosition.y -= 15;
			}
		}
		else
		{
			if (GlobalParameter::upSide)
			{
				//仅朝上
				if (GlobalParameter::imageRightSide)
				{
					hitPosition.x += 20;
					hitPosition.y += 25;
				}
				else
				{
					hitPosition.x -= 20;
					hitPosition.y += 25;
				}
			}
			else
			{
				//仅朝下
				if (GlobalParameter::imageRightSide)
				{
					hitPosition.x += 20;
					hitPosition.y -= 55;
				}
				else
				{
					hitPosition.x -= 20;
					hitPosition.y -= 55;
				}
			}
		}

		actionSprite->setPosition(hitPosition);

		//尝试直接加载
		SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
		frameCache->addSpriteFramesWithFile("KnifeHit.plist", "KnifeHit.png");

		int iFrameNum = 5;
		SpriteFrame* frame = NULL;
		Vector<SpriteFrame*> frameVec;

		for (int i = 1; i <= iFrameNum; i++)
		{
			frame = frameCache->getSpriteFrameByName(StringUtils::format("KnifeHit%d.png", i));
			frameVec.pushBack(frame);
		}

		Animation* animation = Animation::createWithSpriteFrames(frameVec);
		animation->setLoops(1);
		animation->setDelayPerUnit(0.1f);
		Animate* action = Animate::create(animation);


		//回调函数
		CallFunc* callFunc = CallFunc::create(
			[&]() {
			actionSpriteRun[index] = false;
			actionSpriteVec[index]->setVisible(false);
		}
		);

		//组合动作
		Action* actions = Sequence::create(action, callFunc, NULL);
		
		//动作精灵执行动作
		actionSprite->setVisible(true);
		actionSprite->runAction(actions);
	}
}