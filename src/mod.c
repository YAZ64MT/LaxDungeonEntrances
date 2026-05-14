#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "overlays/actors/ovl_Dm_Char01/z_dm_char01.h"
#include "overlays/actors/ovl_En_Dai/z_en_dai.h"
#include "overlays/actors/ovl_Dm_Char08/z_dm_char08.h"

typedef enum EntranceRequirementOption {
    ENTRANCE_REQ_FORM_ONLY,
    ENTRANCE_REQ_SONG_ONLY,
    ENTRANCE_REQ_FORM_AND_SONG,
    ENTRANCE_REQ_FORM_OR_SONG,
    ENTRANCE_REQ_NONE,
} EntranceRequirementOption;

void adjustEntranceRequirements(PlayState *play, PlayerTransformation requiredForm, u16 *checkedOcarinaMode, u8 *checkedTransformation, u16 *checkedLastPlayedSong) {
    Player *player = GET_PLAYER(play);

    switch (recomp_get_config_u32("entrance_requirements")) {
        case ENTRANCE_REQ_FORM_ONLY:
            if (player->transformation == requiredForm) {
                *checkedOcarinaMode = OCARINA_MODE_ACTIVE;
                *checkedLastPlayedSong = play->msgCtx.lastPlayedSong;
            }
            break;

        case ENTRANCE_REQ_SONG_ONLY:
            *checkedTransformation = player->transformation;
            break;

        case ENTRANCE_REQ_FORM_AND_SONG:
            break;

        case ENTRANCE_REQ_FORM_OR_SONG:
            *checkedTransformation = player->transformation;

            if (player->transformation == requiredForm) {
                *checkedOcarinaMode = OCARINA_MODE_ACTIVE;
                *checkedLastPlayedSong = play->msgCtx.lastPlayedSong;
            }
            break;

        case ENTRANCE_REQ_NONE:
            *checkedLastPlayedSong = player->transformation;
            *checkedOcarinaMode = OCARINA_MODE_ACTIVE;
            *checkedLastPlayedSong = play->msgCtx.lastPlayedSong;
            break;

        default:
            break;
    }
}

// Woodfall Temple entrance
extern s16 D_80AAAAB4;

void func_80AA884C(DmChar01 *this, PlayState *play);

RECOMP_PATCH void func_80AA8698(DmChar01 *this, PlayState *play) {
    Player *player = GET_PLAYER(play);

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_20_01)) {
        return;
    }

    if ((player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) && (player->actor.world.pos.x > -40.0f) &&
        (player->actor.world.pos.x < 40.0f) && (player->actor.world.pos.z > 1000.0f) &&
        (player->actor.world.pos.z < 1078.0f)) {
        if (!D_80AAAAB4) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            D_80AAAAB4 = true;
        }
    } else {
        D_80AAAAB4 = false;
    }

    u16 checkedOcarinaMode = OCARINA_MODE_EVENT;
    u8 checkedTransformation = PLAYER_FORM_DEKU;
    u16 checkedLastPlayedSong = OCARINA_SONG_SONATA;

    adjustEntranceRequirements(play, PLAYER_FORM_DEKU, &checkedOcarinaMode, &checkedTransformation, &checkedLastPlayedSong);

    if ((player->transformation == checkedTransformation) && (play->msgCtx.ocarinaMode == checkedOcarinaMode) &&
        (play->msgCtx.lastPlayedSong == checkedLastPlayedSong)) {

        if ((player->actor.world.pos.x > -40.0f) && (player->actor.world.pos.x < 40.0f) &&
            (player->actor.world.pos.z > 1000.0f) && (player->actor.world.pos.z < 1078.0f)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_20_01);
            this->actionFunc = func_80AA884C;
        }
    }
}

typedef enum {
    /* -1 */ ENDAI_ANIM_NONE = -1,
    /*  0 */ ENDAI_ANIM_0,
    /*  1 */ ENDAI_ANIM_1,
    /*  2 */ ENDAI_ANIM_2,
    /*  3 */ ENDAI_ANIM_3,
    /*  4 */ ENDAI_ANIM_4,
    /*  5 */ ENDAI_ANIM_5,
    /*  6 */ ENDAI_ANIM_6,
    /*  7 */ ENDAI_ANIM_7,
    /*  8 */ ENDAI_ANIM_8,
    /*  9 */ ENDAI_ANIM_MAX
} EnDaiAnimation;

// Snowhead Temple entrance
void func_80B3EE8C(EnDai *this, PlayState *play);
s32 func_80B3E96C(EnDai *this, PlayState *play);
s32 EnDai_ChangeAnim(EnDai *this, s32 animIndex);

RECOMP_PATCH void func_80B3EEDC(EnDai *this, PlayState *play) {
    Player *player = GET_PLAYER(play);

    u16 checkedOcarinaMode = OCARINA_MODE_EVENT;
    u8 checkedTransformation = PLAYER_FORM_GORON;
    u16 checkedLastPlayedSong = OCARINA_SONG_GORON_LULLABY;

    adjustEntranceRequirements(play, PLAYER_FORM_GORON, &checkedOcarinaMode, &checkedTransformation, &checkedLastPlayedSong);

    if ((player->transformation == checkedTransformation) && (play->msgCtx.ocarinaMode == checkedOcarinaMode) &&
        (play->msgCtx.lastPlayedSong == checkedLastPlayedSong)) {
        EnDai_ChangeAnim(this, ENDAI_ANIM_1);
        this->actionFunc = func_80B3EE8C;
    } else if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
        func_80B3E96C(this, play);
        this->unk_A6C = 0;
    } else if (this->unk_A6C == 0) {
        Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        this->unk_A6C = 1;
    }
}

// Great Bay Temple entrance
extern s16 sSuccessSoundAlreadyPlayed;

void DmChar08_SetupAppearCs(DmChar08 *this, PlayState *play);

RECOMP_PATCH void DmChar08_WaitForSong(DmChar08 *this, PlayState *play) {
    Player *player = GET_PLAYER(play);

    if ((player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) &&
        ((player->actor.world.pos.x > -5780.0f) && (player->actor.world.pos.x < -5385.0f) &&
         (player->actor.world.pos.z > 1120.0f) && (player->actor.world.pos.z < 2100.0f))) {
        if (!sSuccessSoundAlreadyPlayed) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            sSuccessSoundAlreadyPlayed = true;
        }
    } else {
        sSuccessSoundAlreadyPlayed = false;
    }

    u16 checkedOcarinaMode = OCARINA_MODE_EVENT;
    u8 checkedTransformation = PLAYER_FORM_ZORA;
    u16 checkedLastPlayedSong = OCARINA_SONG_NEW_WAVE;

    adjustEntranceRequirements(play, PLAYER_FORM_ZORA, &checkedOcarinaMode, &checkedTransformation, &checkedLastPlayedSong);

    if ((player->transformation == checkedTransformation) && (play->msgCtx.ocarinaMode == checkedOcarinaMode) &&
        (play->msgCtx.lastPlayedSong == checkedLastPlayedSong)) {
        if ((player->actor.world.pos.x > -5780.0f) && (player->actor.world.pos.x < -5385.0f)) {
            if ((player->actor.world.pos.z > 1120.0f) && (player->actor.world.pos.z < 2100.0f)) {
                this->actionFunc = DmChar08_SetupAppearCs;
            }
        }
    }
}
