#include "Player.h"
#include "Game.h"

Player::Player( void )
{
}

Player::~Player( void )
{
	term();
}

std::string Player::toString( void ) const
{
	return "Player";
}

void Player::init( PlayerIndex index, Vector2 pos /*= Vector2::ZERO*/, Color color /*= Color::WHITE*/, float depth /*= 1.0f */ )
{
	ActiveUnit::init(nullptr, pos, Vector2::ZERO, Vector2::ZERO, Vector2::ZERO, 0.0f, color, depth);

	_playerIndex = index;

	setAnimationKeys();

	switchState(PLAYER_STATE_AIR, VERT_STATE_AIR);

	_gravity = -0.8f;
	_ground = 420.0f;
	_maxSpeed = 8.0f;
	_jumpVelInit = 8.0f;
	_jumpVelMax = 20.0f;
	_jumpVelLeft = _jumpVelMax;
	_damping = 1.0f;
	_airDamping = 1.5f;
	_terminalVel = 25.0f;
	_movementAcc = 2.0f;

	_stateData = &gpDataManager->PlayerStateData["toast"];
}

void Player::term( void )
{
	ActiveUnit::term();
}

void Player::update( const Event& event )
{
	ActiveUnit::update(event);

	//cout << PLAYER_STATE_NAMES[_state] << endl;
	//INF(toString(), Vel.toString());

	if (_vertState == VERT_STATE_AIR)
	{
		Vel.Y -= _gravity;
		if (Vel.Y > _terminalVel)
			Vel.Y = _terminalVel;

		if (_state == PLAYER_STATE_AIR)
		{
			if (sign(Vel.Y) == 1)
			{
				setAnimation(gpDataManager->pAnimations->get("toast-descending"));
			}
			else
			{
				setAnimation(gpDataManager->pAnimations->get("toast-ascending"));
			}
		}

		if (Pos.Y + 255 > _ground) //TODO: Replace with ground checking
		{
			Vel.Y = 0;
			Pos.Y = _ground - 255;
			checkStateData(GAME_INPUT_LANDED);
		}
	}
	else if (_vertState == VERT_STATE_GROUND || _vertState == VERT_STATE_AIR)
	{
		if (abs(Vel.X) < 0.01)
			Vel.X = 0.0f;

		if (Vel.X != 0.0f)
		{
			float damp;

			if (_vertState == VERT_STATE_GROUND)
				damp = _damping;
			else
				damp = _airDamping;

			Vel.X -= damp * sign(Vel.X);
		}
	}
}

void Player::inputPressed( const Event& event )
{
	ActiveUnit::inputPressed(event);

	const InputData* inputData = event.dataAs<InputData>();

	if (inputData->Index != _playerIndex)
		return;

	checkStateData(inputData->Input, GAME_INPUT_TYPE_PRESSED);
}

void Player::inputReleased( const Event& event )
{
	ActiveUnit::inputReleased(event);

	const InputData* inputData = event.dataAs<InputData>();

	if (inputData->Index != _playerIndex)
		return;

	checkStateData(inputData->Input, GAME_INPUT_TYPE_RELEASED);
}

void Player::inputHeld( const Event& event )
{
	ActiveUnit::inputHeld(event);

	const InputData* inputData = event.dataAs<InputData>();

	if (inputData->Index != _playerIndex)
		return;
	
	checkStateData(inputData->Input, GAME_INPUT_TYPE_HELD);

	if (_state == PLAYER_STATE_AIR)
	{
		if (_jumpVelLeft > 0.0f)
		{
			Vel.Y -= 1.0f;
			_jumpVelLeft -= 1.0f;
		}
	}

	if (_state == PLAYER_STATE_WALK || _state == PLAYER_STATE_AIR)
	{
		if (inputData->Input == GAME_INPUT_WEST)
		{
			Vel.X -= _movementAcc;
		}
		else if (inputData->Input == GAME_INPUT_EAST)
		{
			Vel.X += _movementAcc;
		}
		if (abs(Vel.X) > _maxSpeed)
			Vel.X = _maxSpeed * sign(Vel.X);
	}
}

void Player::animationComplete( const Event& event )
{
	ActiveUnit::animationComplete(event);

	if (_state == PLAYER_STATE_JUMPING_START)
	{
		Vel.Y -= _jumpVelInit;
		_jumpVelLeft = _jumpVelMax - _jumpVelInit;
	}

	checkStateData(GAME_INPUT_ANIMATION_COMPLETE);
}

void Player::setAnimationKeys( void )
{
	addAnimationKey(PLAYER_STATE_IDLE,			"toast-idle",				VERT_STATE_GROUND);

	addAnimationKey(PLAYER_STATE_DUCKING_START,	"toast-duck-start",			VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_DUCKING,		"toast-ducking",			VERT_STATE_DUCKING);

	addAnimationKey(PLAYER_STATE_JUMPING_START,	"toast-jump-start",			VERT_STATE_GROUND);

	addAnimationKey(PLAYER_STATE_WALK,			"toast-walk",				VERT_STATE_GROUND);

	addAnimationKey(PLAYER_STATE_GUARD,			"toast-guard",				VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_GUARD,			"toast-ducking-guard",		VERT_STATE_DUCKING);
	addAnimationKey(PLAYER_STATE_GUARD,			"toast-air-guard",			VERT_STATE_AIR);

	addAnimationKey(PLAYER_STATE_LIGHT_PUNCH,	"toast-glp",				VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_LIGHT_KICK,	"toast-glk",				VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_HEAVY_PUNCH,	"toast-ghp",				VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_HEAVY_KICK,	"toast-ghk",				VERT_STATE_GROUND);

	addAnimationKey(PLAYER_STATE_LIGHT_PUNCH,	"toast-dlp",				VERT_STATE_DUCKING);
	addAnimationKey(PLAYER_STATE_LIGHT_KICK,	"toast-dlk",				VERT_STATE_DUCKING);
	addAnimationKey(PLAYER_STATE_HEAVY_PUNCH,	"toast-dhp",				VERT_STATE_DUCKING);
	addAnimationKey(PLAYER_STATE_HEAVY_KICK,	"toast-dhk",				VERT_STATE_DUCKING);

	addAnimationKey(PLAYER_STATE_LIGHT_PUNCH,	"toast-alp",				VERT_STATE_AIR);
	addAnimationKey(PLAYER_STATE_LIGHT_KICK,	"toast-alk",				VERT_STATE_AIR);
	addAnimationKey(PLAYER_STATE_HEAVY_PUNCH,	"toast-ahp",				VERT_STATE_AIR);
	addAnimationKey(PLAYER_STATE_HEAVY_KICK,	"toast-ahk",				VERT_STATE_AIR);

	addAnimationKey(PLAYER_STATE_GRAB,			"toast-grab",				VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_GRAB_MISS,		"toast-grab-miss",			VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_LANDING,		"toast-landing",			VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_KNOCKDOWN,		"toast-knockdown",			VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_RECOVERY,		"toast-recovery",			VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_JUGGLE,		"toast-juggle",				VERT_STATE_AIR);

	addAnimationKey(PLAYER_STATE_HIT_STUN,		"toast-hit-stun",			VERT_STATE_GROUND);
	addAnimationKey(PLAYER_STATE_GUARD_STUN,	"toast-guard-stun",			VERT_STATE_GROUND);

	addAnimationKey(PLAYER_STATE_HIT_STUN,		"toast-ducking-hit-stun",	VERT_STATE_DUCKING);
	addAnimationKey(PLAYER_STATE_GUARD_STUN,	"toast-ducking-guard-stun",	VERT_STATE_DUCKING);

	addAnimationKey(PLAYER_STATE_GUARD_STUN,	"toast-air-guard-stun",		VERT_STATE_AIR);
}

void Player::addAnimationKey( PlayerState state, ItemKey key, VerticalState vertState /*= VERT_STATE_ANY */ )
{
	_animKeys.insert(pair<pair<VerticalState, PlayerState>, ItemKey>(pair<VerticalState, PlayerState>(vertState, state), key));
}

void Player::switchState( PlayerState state /*= INVALID_PLAYER_STATE*/, VerticalState vertState /*= INVALID_VERTICAL_STATE */ )
{
	if (_state == state && _vertState == vertState)
		return;

	if (_state != INVALID_PLAYER_STATE)
		_state = state;

	Game::stateText->setText(PLAYER_STATE_NAMES[_state]);

	if (_vertState != INVALID_VERTICAL_STATE)
		_vertState = vertState;

	pair<VerticalState, PlayerState> statePair(vertState, state);

	if (mapContainsKey(_animKeys, statePair))
	{
		if (!gpDataManager->pAnimations->contains(_animKeys[statePair]))
			return;

		setAnimation(gpDataManager->pAnimations->get(_animKeys[statePair]));
	}
}

void Player::checkStateData( GameInput input, GameInputType type /*= GAME_INPUT_TYPE_OTHER*/ )
{
	pair<GameInput, GameInputType> typePair(input, type);

	if (mapContainsKey(*_stateData, typePair))
	{
		if (mapContainsKey((*_stateData)[typePair], _vertState))
		{
			PlayerStateChangeList list = (*_stateData)[typePair][_vertState];

			for (unsigned int i = 0; i < list.size(); ++i)
			{
				if (list[i].OldState == _state)
				{
					switchState(list[i].NewState, list[i].NewVertState);
				}
			}
		}
	}
}
