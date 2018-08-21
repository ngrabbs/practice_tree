/*
 * practice tree code
 */

/*
 * Need pins for:
 *	1: left_lane_launch
 *	2: right_lane_launch
 *	3: left_lane_pre_stage
 *	LEFT_LANE_BUTTON: right_lane_pre_stage
 *	5: left_lane_stage
 *	6: right_lane_stage
 *	7: pro_tree
 *	8: right_lane_red
 *	9: left_lane_red
 *	10: right_lane_green
 *	11: left_lane_green
 *	12: right_lane_win
 *	13: left_lane_win
 */
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

#define LEFT_LANE_BUTTON 23
#define RIGHT_LANE_BUTTON 24
#define PRO_TREE .400
#define TREE_DELAY 1

#define LL_PRE_LIGHT 0
#define RL_PRE_LIGHT 1
#define LL_STG_LIGHT 2
#define RL_STG_LIGHT 3
#define YELLOWS 4 /* dont have enough for full tree */
#define LL_GREEN_LIGHT 5
#define RL_GREEN_LIGHT 6
#define LL_RED_LIGHT 7
#define RL_RED_LIGHT 8
#define LL_WIN_LIGHT 9
#define RL_WIN_LIHGT 10

int main(int argc, char *argv[])
{
	while(1) {

		/* vars */
		double green_light;
		double tree_start;

		int left_lane_pre_stage = 0;
		int left_lane_stage = 0;
		int left_lane_launched = 0;
		double left_launch;

		int right_lane_pre_stage = 0;
		int right_lane_stage = 0;
		int right_lane_launched = 0;
		double right_launch;

		/* initialise pigpio */
		if(gpioInitialise() < 0) {
			fprintf(stderr, "pigpio initialisation failed\n");
			return 1;
		}

		/* set launch pins to pull up */
		if(gpioSetPullUpDown(LEFT_LANE_BUTTON, PI_PUD_UP) < 0) {
			fprintf(stderr, "failed to set LEFT_LANE_BUTTON pin pull up\n");
			return 1;
		}
		if(gpioSetPullUpDown(RIGHT_LANE_BUTTON, PI_PUD_UP) < 0) {
			fprintf(stderr, "failed to set RIGHT_LANE_BUTTON pin pull up\n");
			return 1;
		}

		/* setup pins */
		/* Set GPIO Modes */
		gpioSetMode(RIGHT_LANE_BUTTON, PI_INPUT);	/* right_lane_launch */
		gpioSetMode(LEFT_LANE_BUTTON, PI_INPUT);	/* left_lane_launch */
		//gpioSetMode(LL_PRE_LIGHT, PI_OUTPUT);	/* left_lane_pre_stage */
		//gpioSetMode(RL_PRE_LIGHT, PI_OUTPUT);	/* right_lane_pre_stage */
		//gpioSetMode(LL_STG_LIGHT, PI_OUTPUT); /* left_lane_stage */
		//gpioSetMode(RL_STG_LIGHT, PI_OUTPUT); /* right_lane_stage */
		//gpioSetMode(YELLOWS, PI_OUTPUT); /* pro_tree */
		//gpioSetMode(LL_GREEN_LIGHT, PI_OUTPUT); /* left_lane_green */
		//gpioSetMode(RL_GREEN_LIGHT, PI_OUTPUT); /* right_lane_green */
		//gpioSetMode(LL_RED_LIGHT, PI_OUTPUT);	/* left_lane_red */
		//gpioSetMode(RL_RED_LIGHT, PI_OUTPUT);	/* right_lane_red */
		//gpioSetMode(LL_WIN_LIGHT, PI_OUTPUT);	/* left_lane_win */
		//gpioSetMode(RL_WIN_LIGHT, PI_OUTPUT);	/* right_lane_win */

		/* wait for pre-stage */
		while(!left_lane_pre_stage || !right_lane_pre_stage) {
			if(!gpioRead(LEFT_LANE_BUTTON) && !left_lane_pre_stage) {
				left_lane_pre_stage = 1;
				printf("left lane prestaged\n");
			}
			if(!gpioRead(RIGHT_LANE_BUTTON) && !right_lane_pre_stage) {
				right_lane_pre_stage = 1;
				printf("right lane prestaged\n");
			}
		}

		/* wait for user to release button */
		while(!gpioRead(LEFT_LANE_BUTTON) && !gpioRead(RIGHT_LANE_BUTTON)) {
		}

		/* wait for stage */
		/* need to add a timer that times out the other lane if they're late
			to stage */
		while(!left_lane_stage || !right_lane_stage) {
			if(!gpioRead(LEFT_LANE_BUTTON) && !left_lane_stage) {
				left_lane_stage = 1;
				printf("left lane staged\n");
			}
			if(!gpioRead(RIGHT_LANE_BUTTON) && !right_lane_stage) {
				right_lane_stage = 1;
				printf("right lane staged\n");
			}
		}

		/* once both have staged start timer */
		/* how long from both have staged to yellow falls */
		/* this could be a random range or preset */
		time_sleep(TREE_DELAY);

		/* drop the tree, set time variables */
		printf("TREE!\n");
		tree_start = time_time();
		
		/* when staged timer expires fire the yellows and */
		/* set the timer for .500 or .LEFT_LANE_BUTTON00 PT */
		green_light = tree_start + PRO_TREE;

		/* check the launch buttons for reaction times */
		while(!left_lane_launched || !right_lane_launched) {
			if(!left_lane_launched && gpioRead(LEFT_LANE_BUTTON)) {
				left_lane_launched = 1;
				left_launch = time_time();
				printf("left lane reaction: %f\n", left_launch - green_light);
			}
			if(!right_lane_launched && gpioRead(RIGHT_LANE_BUTTON)) {
				right_lane_launched = 1;
				right_launch = time_time();
				printf("right lane reaction: %f\n", right_launch - green_light);
			}
		}

		/* check reaction time and fire winner light */
		if (!(left_launch < 0) && (left_launch < right_launch)) {
			printf("Left wins by: %f\n\n", right_launch - left_launch);
		} else if (!(right_launch < 0) && (right_launch < left_launch)) {
			printf("Right wins by %f\n\n", left_launch - right_launch);
		} else {
			printf("Double red probasbly\n\n");
		}



	}
	return 0;
}
