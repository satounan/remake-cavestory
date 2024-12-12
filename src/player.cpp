#include "player.h"
#include "globals.h"
#include "graphics.h"
#include <filesystem>
#include "fstools.h"
#include "sprite.h"

std::filesystem::path neow = std::filesystem::current_path();
std::filesystem::path fix = truncatePathAtDirectoryName(neow,"remake_cavestory");
std::filesystem::path player_path= fix / "content" / "sprites" / "MyChar.png";

namespace player_constants {
	constexpr float WALK_SPEED = 0.2f;
	constexpr float GRAVITY = 0.002f;
	constexpr float GRAVITY_CAP = 0.8f;
}
Player::Player() {}

Player::Player(Graphics &graphics, float x, float y) :
	AnimatedSprite(graphics, player_path.string(), 0, 0, 16, 16, x, y, 100),
	_dx(0),
	_dy(0),
	_facing(RIGHT),
	_grounded(false)
{
	graphics.loadImage(player_path.string());

	this->setupAnimations();
	this->playAnimation("RunRight");
}

void Player::setupAnimations() {
	this->addAnimation(1, 0, 0, "IdleLeft", 16, 16, Vector2(0,0));
	this->addAnimation(1, 0, 16, "IdleRight", 16, 16, Vector2(0,0));
	this->addAnimation(3, 0, 0, "RunLeft", 16, 16, Vector2(0,0));
	this->addAnimation(3, 0, 16, "RunRight", 16, 16, Vector2(0,0));
}

void Player::animationDone(std::string currentAnimation) {}

const float Player::getX() const
{
	return this->_x;
}

const float Player::getY() const
{
	return this->_y;
}


void Player::moveLeft() {
	this->_dx = -player_constants::WALK_SPEED;
	this->playAnimation("RunLeft");
	this->_facing = LEFT;
}

void Player::moveRight() {
	this->_dx = player_constants::WALK_SPEED;
	this->playAnimation("RunRight");
	this->_facing = RIGHT;
}

void Player::stopMoving() {
	this->_dx = 0.0f;
	this->playAnimation(this->_facing == RIGHT ? "IdleRight" : "IdleLeft");
}

void Player::handleTileCollisions(std::vector<Rectangle> &other)
{
	for (auto &rect : other) {
		sides::Side collisionSide = Sprite::getCollisionSide(rect);
		if (collisionSide != sides::Side::NONE) {
			switch (collisionSide) {
				case sides::Side::TOP:
					this->_y = rect.getBottom() + 1;
					this->_dy = 0;
					break;
				case sides::Side::BOTTOM:
					this->_y = rect.getTop() - this->_boundingBox.getHeight() -1;
					this->_dy = 0;
					this->_grounded = true;
					break;
				case sides::Side::LEFT:
					this->_x = rect.getRight() + 1;
					break;
				case sides::Side::RIGHT:
					this->_x = rect.getLeft() - this->_boundingBox.getWidth() - 1;
					break;
				case sides::Side::NONE:
					break;
				
                }
			
		}
	}
}

void Player::update(float elapsedTime) {
	//Move by dx
	if(this->_dy <= player_constants::GRAVITY_CAP)
	{
		this->_dy += player_constants::GRAVITY * elapsedTime;
	}


	this->_x += this->_dx * elapsedTime;
	this->_y += this->_dy * elapsedTime;
	AnimatedSprite::update(elapsedTime);
}

void Player::draw(Graphics &graphics) {
	AnimatedSprite::draw(graphics, this->_x, this->_y);
}
