#include "character/artificial_intelligence.h"

#include <math.h>

#include "view/imageview.h"
#include "data/shareddata.h"
#include "view/timerview.h"
#include "GameManager.h"

enum e_BOSS_001_STATUS_LIST {
    BOSS_STATUS_HEAD_ROTATION_STATUS,                   // progress or reverse
    BOSS_STATUS_HEAD_ROTATION_VALUE,
    BOSS_001_STATUS_HEAD_ROTATION_TIMER,
    BOSS_001_STATUS_HAND1_POS,
    BOSS_001_STATUS_HAND1_TIMER,
    BOSS_001_STATUS_HAND2_POS,
    BOSS_001_STATUS_HAND2_TIMER,
    BOSS_STATUS_HAND_ATTACK_POS_COUNT,
    BOSS_STATUS_HAND_ATTACK_POS_X,
    BOSS_STATUS_HAND_ATTACK_POS_Y,
    BOSS_STATUS_HAND_ATTACK_YINC,
    BOSS_STATUS_HAND_ATTACK_DELAY,
    BOSS_STATUS_HAND_ATTACK_STATUS,


    BOSS_STATUS_HAND_ATTACK_POS_X0,
    BOSS_STATUS_HAND_ATTACK_POS_Y0,







    BOSS_001_STATUS_COUNT
};

enum e_BOSS_001_IMAGE_LIST {
    BOSS_001_IMAGE_HEAD,
    BOSS_001_IMAGE_HEAD_ROTATED,
    BOSS_001_IMAGE_LEAF_RIGHT,
    BOSS_001_IMAGE_LEAF_LEFT,
    BOSS_001_IMAGE_SEGMENT,
    BOSS_001_IMAGE_COUNT
};


enum e_BOSS_001_SPRITE_IMAGE_LIST {
    BOSS_SPRITE_IMAGE_HEAD,
    BOSS_SPRITE_IMAGE_HAND,
    BOSS_SPRITE_IMAGE_COUNT
};

#define HEAD_ANIMATION_DELAY 20
#define HEAD_ANIMATION_MAX_FRAME 10

#define HAND_SPEED 0.07
#define HAND_DELAY 10
#define HAND_ELLIPSIS_W 60
#define HAND_ELLIPSIS_H 30
#define FULL_ROTATION_MAX_VALUE 45 // we'll move 4 degress each time
#define FULL_ROTATION_STEP 360/FULL_ROTATION_MAX_VALUE // we'll move 4 degress each time
#define HAND_ATTACK_SPEED 6.0
#define HAND_ATTACK_MAX_DIST 400

void artificial_intelligence::boss_001_execute_ai() {
    //std::cout << "TEST 001" << std::endl;
}

std::vector<st_rectangle> artificial_intelligence::get_collision_list_boss_001() {
    std::vector<st_rectangle> res;
    // body
    res.push_back(st_rectangle(position.x, position.y, img_boss_body.surface->w, img_boss_body.surface->h));

    // head
    st_imageData *head = &boss_sprite_image_list.at(BOSS_SPRITE_IMAGE_HEAD).at(boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_VALUE));
    res.push_back(st_rectangle(position.x+20, position.y-150, head->surface->w, head->surface->h));

    // left hand
    res.push_back(st_rectangle(position.x+140, position.y+120, boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT).surface->w, boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT).surface->h));

    // right hand
    res.push_back(st_rectangle(position.x+boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X), boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y)+position.y, boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT).surface->w, boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT).surface->h));

    //std::cout << "pos.x[" << position.x << "].y[" << position.y << "], right-hand.x[" << res.at(res.size()-1).x << "].y[" << res.at(res.size()-1).y << "].w[" << res.at(res.size()-1).w << "].h[" << res.at(res.size()-1).h << "]" << std::endl;

    return res;
}

void artificial_intelligence::boss_001_show() {

    if (!is_on_screen()) {
        return;
    }
    //std::cout << "artificial_intelligence::boss_001_show - pos.x[" << position.x << "], real_pos.x[" << realPosition.x << "]" << std::endl;

    if (img_boss_body.is_null()) {
        boss_001_init();
        return;
    }
    character::char_update_real_position();

    // BODY //
    ImageView::get_instance()->renderTexturePortionAt(0, 0, img_boss_body.surface->w, img_boss_body.surface->h, relativePosition.x, relativePosition.y, img_boss_body.texture);


    // HEAD //
    st_imageData *head = &boss_sprite_image_list.at(BOSS_SPRITE_IMAGE_HEAD).at(boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_VALUE));
    int adjust_x = img_boss_head.surface->w - head->surface->w;
    int adjust_y = img_boss_head.surface->h - head->surface->h;
    ImageView::get_instance()->renderTexturePortionAt(0, 0, head->surface->w, head->surface->h, relativePosition.x+adjust_x+20, relativePosition.y+adjust_y-150, head->texture);

    unsigned long now_timer = TimerView::get_instance()->getTimer();
    if (boss_status_list.at(BOSS_001_STATUS_HEAD_ROTATION_TIMER) < now_timer) {
        if (boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_STATUS) == 0.0) {
            boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_VALUE) += 1.0;
            if (boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_VALUE) >= HEAD_ANIMATION_MAX_FRAME) {
                boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_VALUE) -= 1.0;
                boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_STATUS) = 1.0;
            }
        } else {
            boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_VALUE) -= 1.0;
            if (boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_VALUE) <= 0) {
                boss_status_list.at(BOSS_STATUS_HEAD_ROTATION_STATUS) = 0.0;
            }
        }
        boss_status_list.at(BOSS_001_STATUS_HEAD_ROTATION_TIMER) = now_timer + HEAD_ANIMATION_DELAY;
    }



    // HANDS (x2) //
    if (boss_status_list.at(BOSS_001_STATUS_HAND1_TIMER) < now_timer) {
        boss_status_list.at(BOSS_001_STATUS_HAND1_POS) += HAND_SPEED;
        boss_status_list.at(BOSS_001_STATUS_HAND1_TIMER) = now_timer + HAND_DELAY;
        if (boss_status_list.at(BOSS_001_STATUS_HAND1_POS) >= 57) {
            boss_status_list.at(BOSS_001_STATUS_HAND1_POS) = 0;
        }
    }

    // HAND-1 //
    int hand1_x_inc = HAND_ELLIPSIS_W * cos(boss_status_list.at(BOSS_001_STATUS_HAND1_POS));
    int hand1_y_inc = HAND_ELLIPSIS_H * sin(boss_status_list.at(BOSS_001_STATUS_HAND1_POS));


    for (int i=0; i<4; i++) {
        int diff_x0 = ((hand1_x_inc-120)/4)*i;
        int diff_y0 = (hand1_y_inc/4)*i;
        ImageView::get_instance()->renderTexturePortionAt(0, 0, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).surface->w, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).surface->h, relativePosition.x+20+diff_x0, relativePosition.y+120+diff_y0, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).texture);
    }


    ImageView::get_instance()->renderTexturePortionAt(0, 0, boss_image_list.at(BOSS_001_IMAGE_LEAF_LEFT).surface->w, boss_image_list.at(BOSS_001_IMAGE_LEAF_LEFT).surface->h, relativePosition.x-140+hand1_x_inc, relativePosition.y+120+hand1_y_inc, boss_image_list.at(BOSS_001_IMAGE_LEAF_LEFT).texture);




    //std::cout << "HEAD.ROTATION.VALUE[" << (int)boss_status_list.at(BOSS_STATUS_HAND_ROTATION_VALUE) << "], ax[" << boss_sprite_image_list.at(BOSS_SPRITE_IMAGE_HAND).size() << "]" << std::endl;



    // HAND-2 //
    if (boss_status_list.at(BOSS_STATUS_HAND_ATTACK_STATUS) == 0.0) {
        // if starting, calculate the y-inc
        if (boss_status_list.at(BOSS_STATUS_HAND_ATTACK_DELAY) < now_timer) {
            if (boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_COUNT) == 0.0) {
                int dist_x = HAND_ATTACK_MAX_DIST;
                boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X0) = boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X);
                boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y0) = boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y);


                int dist_y = boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) - GameManager::get_instance()->get_player_center_position().y;

                std::cout << ">>> LEAF.Y[" << boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) << "], p.y[" << GameManager::get_instance()->get_player_center_position().y << "], dist_y[" << dist_y << "]" << std::endl;

                boss_status_list.at(BOSS_STATUS_HAND_ATTACK_YINC) = ((float)dist_y/(float)dist_x)*HAND_ATTACK_SPEED;
                boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) = 180;
                //std::cout << ">>>>>>>>>>> dist_x[" << dist_x << "], dist_y[" << dist_y << "], y_inc[" << boss_status_list.at(BOSS_STATUS_HAND_ATTACK_YINC) << "]" << std::endl;

            }
            boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_COUNT) += HAND_ATTACK_SPEED;
            boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) -= boss_status_list.at(BOSS_STATUS_HAND_ATTACK_YINC);
            //std::cout << "INC.BOSS_STATUS_HAND_ATTACK_POS_Y[" << boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) << "]" << std::endl;
            if (boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_COUNT) >= HAND_ATTACK_MAX_DIST) {
                boss_status_list.at(BOSS_STATUS_HAND_ATTACK_STATUS) = 1.0;
            }
        } else { // while waiting next attack, act as ellipsis movement

        }
    } else {
        boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_COUNT) -= HAND_ATTACK_SPEED;
        boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) += boss_status_list.at(BOSS_STATUS_HAND_ATTACK_YINC);
        if (boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_COUNT) <= 0) {
            boss_status_list.at(BOSS_STATUS_HAND_ATTACK_DELAY) = now_timer + (rand() % 2000);

            boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_COUNT) = 0.0;
            boss_status_list.at(BOSS_STATUS_HAND_ATTACK_STATUS) = 0.0;
        }
    }
    //std::cout << "BOSS_STATUS_HAND_ATTACK_POS_Y[" << boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) << "]" << std::endl;
    if (boss_status_list.at(BOSS_STATUS_HAND_ATTACK_DELAY) < now_timer) {
        boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X) = boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X0) + boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_COUNT);
        boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) = boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y0);
        //ImageView::get_instance()->renderTexturePortionAt(0, 0, boss_image_list.at(BOSS_001_IMAGE_LEAF).surface->w, boss_image_list.at(BOSS_001_IMAGE_LEAF).surface->h, realPosition.x+180+boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X), boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y), boss_image_list.at(BOSS_001_IMAGE_LEAF).texture);
    } else {
        if (boss_status_list.at(BOSS_001_STATUS_HAND2_TIMER) < now_timer) {
            boss_status_list.at(BOSS_001_STATUS_HAND2_POS) += HAND_SPEED;
            boss_status_list.at(BOSS_001_STATUS_HAND2_TIMER) = now_timer + HAND_DELAY;
            if (boss_status_list.at(BOSS_001_STATUS_HAND2_POS) >= 57) {
                boss_status_list.at(BOSS_001_STATUS_HAND2_POS) = 0;
            }
        }
        int hand2_x_inc = HAND_ELLIPSIS_W * cos(boss_status_list.at(BOSS_001_STATUS_HAND2_POS));
        int hand2_y_inc = HAND_ELLIPSIS_H * sin(boss_status_list.at(BOSS_001_STATUS_HAND2_POS));
        boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X) = 140+hand2_x_inc;
        boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) = 120+hand2_y_inc;
    }


    // draw segments
    for (int i=0; i<4; i++) {
        int origin_x = 140;
        int origin_y = 120;
        int diff_x0 = ((boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X) - origin_x)/4)*i;
        int diff_y0 = ((boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y) - origin_y)/4)*i;
        //ImageView::get_instance()->renderTexturePortionAt(0, 0, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).surface->w, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).surface->h, boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X0)+diff_x0, boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y0)+diff_y0, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).texture);
        ImageView::get_instance()->renderTexturePortionAt(0, 0, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).surface->w, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).surface->h, relativePosition.x+origin_x+diff_x0, relativePosition.y+origin_y+diff_y0, boss_image_list.at(BOSS_001_IMAGE_SEGMENT).texture);
    }

    ImageView::get_instance()->renderTexturePortionAt(0, 0, boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT).surface->w, boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT).surface->h, relativePosition.x+boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_X), relativePosition.y+boss_status_list.at(BOSS_STATUS_HAND_ATTACK_POS_Y), boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT).texture);


}

void artificial_intelligence::boss_001_init() {
    // TODO: use a list
    img_boss_body = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/bosses/plant_body.png");
    img_boss_head = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/bosses/plant_head.png");
    img_boss_segment = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/bosses/plant_segment.png");


    boss_image_list.insert(std::pair<int,st_imageData>(BOSS_001_IMAGE_HEAD_ROTATED, st_imageData()));
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HEAD_ROTATION_STATUS, 0.0));
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HEAD_ROTATION_VALUE, 0.0));
    boss_status_list.insert(std::pair<int,float>(BOSS_001_STATUS_HEAD_ROTATION_TIMER, 0.0));


    // HEAD - store rotated images //
    boss_sprite_image_list.insert(std::pair<int, std::vector<st_imageData>>(BOSS_SPRITE_IMAGE_HEAD, std::vector<st_imageData>()));
    for (int i=0; i<HEAD_ANIMATION_MAX_FRAME; i++) {
        // TODO - implement in SDL3 //
        //boss_sprite_image_list.at(BOSS_SPRITE_IMAGE_HEAD).push_back(ImageView::get_instance()->rotated_from_image(img_boss_head, i));
    }


    // HANDS (x2) //
    boss_status_list.insert(std::pair<int,float>(BOSS_001_STATUS_HAND1_POS, 0.0));
    boss_status_list.insert(std::pair<int,float>(BOSS_001_STATUS_HAND1_TIMER, 0.0));
    boss_status_list.insert(std::pair<int,float>(BOSS_001_STATUS_HAND2_POS, 3.0));
    boss_status_list.insert(std::pair<int,float>(BOSS_001_STATUS_HAND2_TIMER, 0.0));
    boss_image_list.insert(std::pair<int,st_imageData>(BOSS_001_IMAGE_LEAF_RIGHT, st_imageData()));
    boss_image_list.at(BOSS_001_IMAGE_LEAF_RIGHT) = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/bosses/plant_leaf.png");
    boss_image_list.insert(std::pair<int,st_imageData>(BOSS_001_IMAGE_LEAF_LEFT, st_imageData()));
    boss_image_list.at(BOSS_001_IMAGE_LEAF_LEFT) = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/bosses/plant_leaf_left.png");

    // hand attack //
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_YINC, 0.0));
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_DELAY, 0.0));
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_POS_COUNT, 0.0));

    character::char_update_real_position();
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_POS_X0, 140));
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_POS_Y0, 120));
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_POS_X, 140));
    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_POS_Y, 120));


    boss_status_list.insert(std::pair<int,float>(BOSS_STATUS_HAND_ATTACK_STATUS, 0.0));
    boss_sprite_image_list.insert(std::pair<int, std::vector<st_imageData>>(BOSS_SPRITE_IMAGE_HAND, std::vector<st_imageData>()));

    // SEGMENTS //
    boss_image_list.insert(std::pair<int,st_imageData>(BOSS_001_IMAGE_SEGMENT, st_imageData()));
    boss_image_list.at(BOSS_001_IMAGE_SEGMENT) = ImageView::get_instance()->imageFromFile(SharedData::get_instance()->FILEPATH + "/images/sprites/enemies/bosses/plant_segment.png");
}
