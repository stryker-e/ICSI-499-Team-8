//https://github.com/0xfauzi/Wave-front-algorithm-implementation
//https://www.societyofrobots.com/programming_wavefront.shtml
/************************ COMPUTE NEIGHBOR *************************/
void computeNeighbor(Cell &currCell)
{
    northNeigh.x = currCell.x;
    northNeigh.y = currCell.y - 1;
    //If coordinates of north neighbor are valid, northVal is the value of north neighbor, else it's invalid
    checkCoordinate(northNeigh.x, northNeigh.y) == true ? northVal = mapMatrix[northNeigh.y][northNeigh.x] : northVal = -1;
    if(WavefrontTesting){
        Serial.print("north neighbor is: ");
        Serial.print(northNeigh.x);
        Serial.print(", ");
        Serial.print(northNeigh.y);
        Serial.print("Value is: ");
        Serial.println(northVal);
    }

    southNeigh.x = currCell.x;
    southNeigh.y = currCell.y + 1;
    checkCoordinate(southNeigh.x, southNeigh.y) == true ? southVal = mapMatrix[southNeigh.y][southNeigh.x] : southVal = -1;
    if(WavefrontTesting){
        Serial.print("south neighbor is: ");
        Serial.print(southNeigh.x);
        Serial.print(", ");
        Serial.print(southNeigh.y);
        Serial.print("Value is: ");
        Serial.println(southVal);
    }

    eastNeigh.x = currCell.x + 1;
    eastNeigh.y = currCell.y;
    checkCoordinate(eastNeigh.x, eastNeigh.y) == true ? eastVal = mapMatrix[eastNeigh.y][eastNeigh.x] : eastVal = -1;
    if(WavefrontTesting){
        Serial.print("east neighbor is: ");
        Serial.print(eastNeigh.x);
        Serial.print(", ");
        Serial.print(eastNeigh.y);
        Serial.print("Value is: ");
        Serial.println(eastVal);
    }

    westNeigh.x = currCell.x - 1;
    westNeigh.y = currCell.y;
    checkCoordinate(westNeigh.x, westNeigh.y) == true ? westVal = mapMatrix[westNeigh.y][westNeigh.x] : westVal = -1;
    if(WavefrontTesting){
        Serial.print("west neighbor is: ");
        Serial.print(westNeigh.x);
        Serial.print(", ");
        Serial.print(westNeigh.y);
        Serial.print("Value is: ");
        Serial.println(westVal);
    }
}

void setGoal()
{
  int counter = 0;
  for (int i = 0; i < Y_SIZE; i++)
  {
    for (int j = 0; j < X_SIZE; j++)
    {
      if (counter < mapMatrix[i][j])
      {
        counter = mapMatrix[i][j];
        goal_x = j;
        goal_y = i;
      }
    }
  }

  Serial.print("The goal is set to ");
  Serial.print(goal_x);
  Serial.print(" ");
  Serial.println(goal_y);
}

/******************** COMPUTE PATH ************************************/
void computePath(int numCols, int numRows, int startX, int startY)
{
  // Initialize start positions
  Cell currCell;
  currCell.x = startX;
  currCell.y = startY;

  int smallestCell = 0;

  bool pathFound;

  // compare all neighbors to find the smallest which is not OBST

  int i = 0;
  while (smallestCell != mapMatrix[goal_y][goal_x])
  {
    pathFound = false;
    Serial.print(i);
    Serial.print(" ");
    Serial.println(sizeof(direction));
    if (i > sizeof(direction))
    {
      // Create new array with bigger size
      int newDirections[sizeof(direction) * 2];
      for (int i = 0; i < sizeof(direction); i++)
      {
        newDirections[i] = direction[i];
      }
      memcpy(direction, newDirections, sizeof(direction));
    }
    computeNeighbor(currCell); // Compute and set all possible neighbors
    // Check to make sure the neighbors are valid
    // Assign arbitrary smallest cell for comparison
    //smallestCell = mapMatrix[currCell.y][currCell.x];
    if(WavefrontTesting){
        Serial.print("temp smallestCell value: ");
        Serial.println(smallestCell);
    }

    while (!pathFound)
    {

      if (northVal < smallestCell && (northVal != WALL && northVal != -1) && !boolMap[currCell.x][currCell.y - 1])
      {
        smallestCell = northVal;
        Serial.println(northVal);
        // found best cell...set current cell to that cexl
        currCell.x = northNeigh.x;
        currCell.y = northNeigh.y;
        direction[i] = 1;
        pathFound = true;
      }
      if (southVal < smallestCell && (southVal != WALL && southVal != -1) && !boolMap[currCell.x][currCell.y + 1])
        {
          smallestCell = southVal;
          Serial.println(southVal);
          // found best cell...set current cell to that cell
          currCell.x = southNeigh.x;
          currCell.y = southNeigh.y;
          direction[i] = 2;
          pathFound = true;
        }
      if (eastVal < smallestCell && (eastVal != WALL && eastVal != -1) && !boolMap[currCell.x + 1][currCell.y])
        {
          smallestCell = eastVal;
          Serial.println(eastVal);
          // found best cell...set current cell to that cell
          currCell.x = eastNeigh.x;
          currCell.y = eastNeigh.y;
          direction[i] = 3;
          pathFound = true;
        }
      if (westVal < smallestCell && (westVal != WALL && westVal != -1) && !boolMap[currCell.x - 1][currCell.y])
          {
            smallestCell = westVal;
            Serial.println(westVal);
            // found best cell...set current cell to thay cell
            currCell.x = westNeigh.x;
            currCell.y = westNeigh.y;
            direction[i] = 4;
            pathFound = true;
          }
       if (!pathFound){
         smallestCell++;
         Serial.println(smallestCell);
       }
    }
    // Print
    // Serial.println("Moving to cell: %d,%d | Value: %d",currCell.x,currCell.y, mapMatrix[currCell.y][currCell.x]);
    if(WavefrontTesting){
        Serial.print("Moving to cell: ");
        Serial.print(currCell.x);
        Serial.print(" ");
        Serial.println(currCell.y);
        Serial.print("The current smallest cell is: ");
        Serial.print(currCell.x);
        Serial.print(" ");
        Serial.println(currCell.y);
    }
    // mark current cell as visited in boolean matrix map
    boolMap[currCell.x][currCell.y] = 1;
    followPlan(i);
    i++;
  }
  verifyCoverage();
  if(WavefrontTesting){
    for (int j = 0; j < sizeof(direction); j++)
    {
        Serial.print(direction[j]);
        Serial.print(" ");
    }
  }
}
/******************************** END COMPUTE PATH *********************************/

/******************** CHECK COORDINATES *************************************/
bool checkCoordinate(int x, int y)
{
  bool returnValue;
  int dummy = 0;
  int dummy2 = 0;

  x == -1 ? returnValue = false : dummy += 1; // If x is invalid, return value is false and increment dummy by 1
  y == -1 ? returnValue = false : dummy += 2; // If y is invalid return value is false and increment dummy by `

  x > (X_SIZE - 1) ? returnValue = false : dummy2 += 3; // If x exceeds boundary return value is false, increment dummy2 by 3
  y > (Y_SIZE - 1) ? returnValue = false : dummy2 += 4; // If y exceeds boundary return value is false, increment dummy2 by 4

  // If dummy is 3 and dumm2 is 7, then both tests have passed, coordinates are valid
  if (dummy == 3 && dummy2 == 7)
  {
    //    Serial.print(x);
    //    Serial.print(" ");
    //    Serial.println(y);
    //    Serial.println("True");
    return true;
  }
  else // Otherwise tests failed, return false
  {
    //    Serial.print(x);
    //    Serial.print(" ");
    //    Serial.println(y);
    //    Serial.println("False");
    return returnValue;
  }
}
/*********************** END CHECK COORDINATES ******************************/

/*********************** FOLLOW PLAN *************************************************/
void followPlan(int i)
{
  //********************   Direction to move to: NORTH    ***************//
  // Orientation: NORTH
  if (direction[i] == 1 && orientation == NORTH)
  {
    move1Square();
    if(WavefrontTesting){
        Serial.println("Orientation is still NORTH");
    }
  }
  // Orientation: SOUTH
  else if (direction[i] == 1 && orientation == SOUTH)
  {
    turn90CW();
    turn90CW();
    move1Square();
    orientation = NORTH; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is NORTH");
    }
  }
  // Orientation: EAST
  else if (direction[i] == 1 && orientation == EAST)
  {
    turn90CCW();
    move1Square();
    orientation = NORTH; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is NORTH");
    }
  }
  // Orientation: WEST
  else if (direction[i] == 1 && orientation == WEST)
  {
    turn90CW();
    move1Square();
    orientation = NORTH; // Reset orientation
    if(WavefrontTesting){
        Serial.println("Orientation is NORTH");
    }
  }

  //********************  Direction to move to: SOUTH    ***************//
  // Orientation: SOUTH
  if (direction[i] == 2 && orientation == SOUTH)
  {
    move1Square();
    if(WavefrontTesting){
        Serial.println("Orientation is still SOUTH");
    }
  }
  // Orientation: NORTH
  else if (direction[i] == 2 && orientation == NORTH)
  {
    turn90CW();
    turn90CW();
    move1Square();
    orientation = SOUTH; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is SOUTH");
    }
  }
  // Orientation: EAST
  else if (direction[i] == 2 && orientation == EAST)
  {
    turn90CW();
    move1Square();
    orientation = SOUTH; // Reset orientation
    if(WavefrontTesting){
        Serial.println("Orientation is SOUTH");
    }
  }
  // Orientation: WEST
  else if (direction[i] == 2 && orientation == WEST)
  {
    turn90CCW();
    move1Square();
    orientation = SOUTH; // Reset orientation
    if(WavefrontTesting){
        Serial.println("Orientation is SOUTH");
    }
  }

  //******************** Direction to move to: EAST     ***************//
  // Orientation: EAST
  if (direction[i] == 3 && orientation == EAST)
  {
    move1Square();
    if(WavefrontTesting){
        Serial.println("Orientation is still EAST");
    }
  }
  // Orientation: NORTH
  else if (direction[i] == 3 && orientation == NORTH)
  {
    turn90CW();
    move1Square();
    orientation = EAST; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is EAST");
    }
  }
  // ORIENTATION: SOUTH
  else if (direction[i] == 3 && orientation == SOUTH)
  {
    turn90CCW();
    move1Square();
    orientation = EAST; // Reset orientation
    if(WavefrontTesting){
        Serial.println("Orientation is EAST");
    }
  }
  // ORIENTATION: WEST
  else if (direction[i] == 3 && orientation == WEST)
  {
    turn90CW();
    turn90CW();
    move1Square();
    orientation = EAST; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is EAST");
    }
  }

  //********************  Direction to move to: WEST     ***************//
  // Orientation: WEST
  if (direction[i] == 4 && orientation == WEST)
  {
    move1Square();
    if(WavefrontTesting){
        Serial.println("Orientation is still WEST");
    }
  }
  // Orientation: NORTH
  else if (direction[i] == 4 && orientation == NORTH)
  {
    turn90CCW();
    move1Square();
    orientation = WEST; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is WEST");
    }
  }
  // Orientation: WEST
  else if (direction[i] == 4 && orientation == SOUTH)
  {
    turn90CW();
    move1Square();
    orientation = WEST; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is WEST");
    }
  }
  // Orientation: EAST
  else if (direction[i] == 4 && orientation == EAST)
  {
    turn90CCW();
    turn90CCW();
    move1Square();
    orientation = WEST; // Reset Orientation
    if(WavefrontTesting){
        Serial.println("Orientation is WEST");
    }
  }
}

//If the roomba missed some cells while on the way to the goal
//Find the cell with the lowest # and set it as the new goal
//Then call computePath with the previous goal as the start point
void verifyCoverage(){
   int smallestValue = mapMatrix[goal_y][goal_x];
   Cell temp;
   Cell prevGoal;
   for (int i = 0; i < Y_SIZE; i++)
    {
    for (int j = 0; j < X_SIZE; j++)
    {
      if(mapMatrix[i][j] > 0 && boolMap[i][j] != 0){
        if(mapMatrix[i][j] < smallestValue){
          smallestValue = mapMatrix[i][j];
          temp.x = j;
          temp.y = j;
        }
      }
    }
  }
   if(smallestValue != mapMatrix[goal_y][goal_x])
    {
      prevGoal.x = goal_x;
      prevGoal.y = goal_y;
      goal_x = temp.x;
      goal_y = temp.y;
    }
    computePath(X_SIZE, Y_SIZE, prevGoal.x, prevGoal.y);
}

void generateCells()
{
  Cell cell = {start_x, start_y};
  q.push(&cell);

  while (!q.isEmpty())
  {
    q.peek(&nextCell);

    Cell temp;
    temp.x = nextCell.x;
    temp.y = nextCell.y;

    computeNeighbor(temp);
    // display();
    // End
    q.peek(&temp);
    int y = temp.y;
    int x = temp.x;

    if (northVal == 0)
    {
      mapMatrix[northNeigh.y][northNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = northNeigh.x;
      temp.y = northNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    if (southVal == 0)
    {
      mapMatrix[southNeigh.y][southNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = southNeigh.x;
      temp.y = southNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    if (eastVal == 0)
    {
      mapMatrix[eastNeigh.y][eastNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = eastNeigh.x;
      temp.y = eastNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    if (westVal == 0)
    {
      mapMatrix[westNeigh.y][westNeigh.x] = mapMatrix[y][x] + 1;
      temp.x = westNeigh.x;
      temp.y = westNeigh.y;
      //      Serial.print(temp.x);
      //      Serial.print(" ");
      //      Serial.print(temp.y);
      //      Serial.print(" ");
      //      Serial.println(counter++);
      q.push(&temp);
      // display();
    }
    q.drop();
  }
  setGoal();
  printMaptoSerial();
}