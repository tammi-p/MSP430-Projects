Simon Game

Description:
The Simon Game is a device that plays a series of tones and lights, which the player needs to repeat. This sequence becomes longer and longer for each turn that the player correctly repeats it.

Components: 4 buttons with corresponding LEDs and tones

How it works: 
1. The device, called "Simon," will play a tone and light up an LED corresponding to pressing a random button
2. Player must repeat the sequence Simon played by pushing the correct button
3. If player succeeds, the sequence becomes longer by 1 tone/light, player must repeat the new sequence, and so on
4. A maximum sequence length is set in the code
  a. If player succeeds to last round -> WIN
  b. If player presses wrong button or waits too long to press a button (timeout) -> LOSE
  * WIN and LOSE are distinguished by unique tones and light sequences
  
Main parts of the code:
  * State 0 - LOSE sequence is played until any button is pressed
  * State 1 - Simon's turn and player's turn
  * State 2 - WIN sequence is played until any button is pressed
  
State 0:
1. A random button sequence is generated
2. LOSE sequence is played until any button is pressed -> State 1

State 1:
* A variable called turn, initialized to 1, keeps track of the turn number
1. Simon plays (length of sequence played depends on the value of the variable turn)
2. If timed out or player pressed wrong button -> State = 0
3. If player plays entire sequence correctly -> State = 2
4. Else, (player played correct sequence but we are not at the final turn yet) -> turn is incremented by 1 and State 1 is repeated

State 2:  
1. Play WIN sequence
2. If a button is pressed, generate a random sequence and State = 1.
* Pressing a button enters an ISR, which contains code checking if button pressed is correct
