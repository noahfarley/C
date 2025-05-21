#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void beginning(double *bankroll, int gamerecord[], double *temp);

void playing(double *bankroll, double *bet, int gamerecord[], double *lastbet);
int dealing(void);
int total(int hand[]);
int value(int card);
void face(int card);
void faceall(int hand[]);

void report(double bankroll, double temp, double lastbet, int gamerecord[]);
void ending(double bankroll, int gamerecord[], double temp, double lastbet);

int main (void) {
	
	printf("\n");
	int gamerecord[] = {0,0,0,0}; //won, lost, blackjack, busted.
	double bankroll = 0; //new bankroll
	double temp = 0; //old bankroll
	double bet = 0; //bet
	double lastbet = 0; //last played bet

	srandom((unsigned int)(time(NULL))); //make random numbers
		
	beginning(&bankroll, gamerecord, &temp); //load data or create data with $1000 and default
	
	/*if(bankroll<10) { //need more money
		printf("You have %.2f and starting bets are $10\nedit save file for more money with:\nvi savefile.txt\n",bankroll);
		return 0;
	}*/ //moved to ending();
		
	do {
		playing(&bankroll, &bet, gamerecord, &lastbet); //playing the game
		//report(bankroll, temp, bet, gamerecord); //outcome of the game
		//moved to ending();
	} while(bankroll>=10 && bet>=10); //conditions for the game

	/*if(bankroll<10) { //need more money, gambled it all away
		printf("You just lost and now you have %.2f and starting bets are $10\nedit save file for more money with:\nvi savefile.txt\n",bankroll);
	}*/ //moved to ending
	
	ending(bankroll, gamerecord, temp, lastbet); //save player data
	return 0;
}

void beginning(double *bankroll, int gamerecord[], double *temp) {
        
	FILE *sfr; //save file read
	int status = 0; //lines
	
	sfr = fopen("savefile.txt","r");//read

	if(sfr != NULL) {
		status = fscanf(sfr,"%lf %d %d %d %d", bankroll, &gamerecord[0], &gamerecord[1], &gamerecord[2], &gamerecord[3]);
		fclose(sfr);
		printf("save file loaded\n");
	} else {
		//fclose(sfr); //core dump err
		FILE *sfw; //save file write
		*bankroll = 1000;
		sfw = fopen("savefile.txt","w");
		fprintf(sfw,"%.2f %d %d %d %d", 1000.00, gamerecord[0], gamerecord[1], gamerecord[2], gamerecord[3]);
		//fclose(sfw); //core dump err
		printf("new save created\n");
	}
	*temp = *bankroll; //current saved money = staring money
}

void playing(double *bankroll, double *bet, int gamerecord[], double *lastbet){
        
	int player[21];
	int dealer[21];
	//max amount of cards

	char p, d; //conditions

	for(int e = 0; e<21; e++) { //makes all cards 0
		player[e] = 0;
		dealer[e] = 0;
	}

	if(*bankroll<10) { //need more money
		/*printf("You have %.2f and starting bets are $10\nedit save file for more money with:\nvi savefile.txt\n", *bankroll);*/
		return;
	}

        printf("\nbet: from 10 to %.2f\nbet < 10 for quit: ", *bankroll);
        scanf("%lf", bet);
       
	if (*bet > *bankroll) {
		printf("\nCannot bet more than you have\n");
		*bet = 0;
	}

        if (*bet < 10) {
                printf("exit round\n");
                return;
	}

	*lastbet = *bet;
	
	//creates new cards
	player[0] = dealing();
	dealer[0] = dealing();
	player[1] = dealing();
	dealer[1] = dealing();

	printf("\nDealer's cards: \n");
	face(dealer[0]);
	//face(dealer[1]);
	printf("#\n");
	printf("Dealer's total: ???\n");

	printf("\nYour cards: \n");
	face(player[0]);
	face(player[1]);
	printf("Your total: %d\n",total(player));
	printf("\n");
	
	p = 'h'; //h = hit
	d = 'h';

	if(total(player) == 21 || total(dealer) == 21) { //total() value of all cards from hand
		p = 's';
		d = 's';
	} //a = no hitting //push

	for (int i = 2; (total(player)<21) && (p =='h'); i++) {
		printf("h for hit and s for stand: ");
		p = 'h';
		scanf(" %c", &p);
		
		if(p == 'h') {
			player[i] = dealing();
			printf("New card: ");
			face(player[i]);
			printf("Your total: %d\n",total(player));
		}
	}

	printf("\nDealer's turn\n");

	for (int i = 2; total(dealer)<17 && d == 'h' ; i++) {
		dealer[i] = dealing();
		printf("New Dealer's card: ");
		face(dealer[i]);
	}

	printf("\nDealer's cards: \n");
	faceall(dealer); //faceall() shows all cards from hand[]
	printf("Dealer's total: %d\n",total(dealer));

	printf("\nYour Cards: \n");
	faceall(player);
	printf("Your total: %d\n",total(player));
		
	printf("end of round\n\n");

	if((total(dealer)>21 && !(total(player)>21)) || ((total(dealer)<total(player)) && total(player)<22)) {
		printf("You Win!\n"); //win
		if(value(player[0]) + value(player[1]) == 21) {
			printf("1.5x for blackjack!\n");
			gamerecord[2]++; //record blackjack
			*bet = *bet + *bet/2.0; //1.5 bet for blackjack
		}
		*bankroll += *bet; //add bet to bankroll
		gamerecord[0]++; //record win
	} else if ((total(player)>21 && !(total(dealer)>21)) || ((total(dealer)>total(player)) && total(dealer)<22)) {
		printf("You Lose!\n"); //lose
		double minus = -(*bet); //losing the bet
		*bankroll += minus;
		gamerecord[1]++; //lose record
		if(total(player)>21) {
			gamerecord[3]++; //record bust
		}
	} else {
	       	printf("Draw!\n"); //tie
		//*bet = 0; //testing
	}
}

int dealing(void) { //makes rand form 1 to 13
	int random_n;
	random_n = 1 + (random() % 13);
	return random_n;
}

int total(int hand[]) { //total value() of hand
	
	int temp2 = 0; //return value
	int flag = 0; //ace card
	int index[21]; //ace card location

	do{
		if (flag) { //change first ace in hand from value 11 to 1
			for(int n = 0; n<21 && value(hand[n]) && flag; n++) {
				if (index[n]) {
					hand[n] = 14;
					flag = 0;
				}
			}
		}
		
		flag = 0; //reset ace var
		temp2 = 0; //reset return

		for(int k = 0; k<21 && value(hand[k]); k++) { //adds value() of hand[]
			if(hand[k]==1) { //ace check
				flag = 1;
				index[k] = 1;
				if((temp2 + value(hand[k])) > 21) {//if ace goes over change it
					hand[k] = 14;
				}
			}
			temp2 = temp2 + value(hand[k]); //add hand[i] to current total
		}
		
	} while (flag && (temp2 > 21)); //change ace value from 1 to 11 if done again
	
	return temp2;
}

int value(int card) { 
	int temp3; //return value

	switch(card) {
		case 1:
			temp3=11; //high ace: 11
			break;
		case 11:
		case 12:
		case 13:
			temp3=10; //face cards
			break;
		case 14:
			temp3=1; //low ace: 1
			break;
		default:
			temp3 = card; //number cards
	}

	return temp3;
}


void face(int card) { //print cards

	switch(card) {
		case 1:
		case 14:
			printf("A\n");
			break;
		case 11:
			printf("J\n");
			break;
		case 12:
			printf("Q\n");
			break;
		case 13:
			printf("K\n");
			break;
		default:
			printf("%d\n",card);
	}
}

void faceall(int hand[]) { //shows all cards in hand
	for(int l = 0; value(hand[l]); l++){
		face(hand[l]);
	}
}

void report(double bankroll, double temp, double lastbet, int gamerecord[]){
	//if(bet<10) {return;} //to make sure report() does not display twice
	//not used bc report moved to ending();

	printf("New bankroll: %.2f\tOld bankroll: %.2f\nmoney earned so far: %.2f\tlast bet played: %.2f\n", bankroll, temp, bankroll-temp, lastbet);
	printf("wins: %d\nlosses:%d\nblackjacks: %d\nbusts: %d\n",gamerecord[0], gamerecord[1], gamerecord[2], gamerecord[3]);

}

void ending(double bankroll, int gamerecord[], double temp, double lastbet) {

	report(bankroll, temp, lastbet, gamerecord); //outcome of the game

	FILE *sfw; //save file write
	
	sfw = fopen("savefile.txt","w");
	fprintf(sfw,"%.2f %d %d %d %d", bankroll, gamerecord[0], gamerecord[1], gamerecord[2], gamerecord[3]);
	//fclose(sfw); //save not saved err
	printf("data saved\n");

	if(bankroll<10) { //need more money
		printf("You have %.2f and starting bets are $10\nedit savefile.txt for more money with:\nvi savefile.txt\n",bankroll);
	}
}
