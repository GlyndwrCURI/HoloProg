#include "stdafx.h"

extern char mes[];

extern double ldblArrayOfGonioAngles[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];//ie [MAXROWS][MAXCOLS]  Note: although global, no data is passed between source modules / functions.  GenerateProfileForM11() initialises
// it, uses it, then forgets it.  GenerateProfileForM12()does the same... It is just working storage.  To conserve memory, I'll use this same space for M7,8,9&10 too... Here it's not an Array Of GonioAngles, but of
// motor positions in millimeters.  They are still stored as doubles - really the table should be renamed, globally, but ldblArrayOfGonioAnglesOrMotorPositionsInMillimeters is rather long!

//To Avoid duplicating code, I'll also use this routine to read in Energy Files. M3, M15, M27 (and now M2, M14, M26 too, if the object beam needs its energy to be varied). In this instance, I'll memcpy() 
// ldblArrayOfGonioAngles to dblArDesiredEnergyWP3r after the file is read in (if bCopyToEnergyArray is TRUE).
extern double dblArDesiredEnergyWP3r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP2r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP2g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP2b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];

//********************************************************************************************* Function m yOpen_File_VariableAp_CSV() ***********************************************************************************//
//                                                                                                                                                                                                                        //
// m yOpen_File_VariableAp_CSV's job is to load the ldblArrayOfGonioAngles table with data read from a spreadsheet that is exported as a csv file.  A call to this function replaces the block of code in                 //
// GenerateProfileForM12() where the loops "for each row, for each column" repeatedly call the line:                                                                                                                      //
//      ldblArrayOfGonioAngles[row][col] = GetWyInDegreesFromSpreadSheet(PixelHpositionInCms + dblHalfAHorzPixel, PixelVpositionInCms);                                                                                   //
//                                                                                                                                                                                                                        //
// Optic may, at some point when the details of the spreadsheet's calculation have been finalised, code GetWyInMMFromSpreadSheetExportFile() with that calculation and so remove m yOpen_File_VariableAp_CSV()            //
//                                                                                                                                                                                                                        //
//************************************************************************************************************************************************************************************************************************//

//nGONIO_COL_IDX is a poor name for the variable which really is just saying if the motor is red green or blue (it's just copied from code in other places, which was originally to do with the GONIOs).
// The last three arguments are only used if we're reading an Energy file.
int myOpen_File_VariableAp_CSV(const char *pszFile2Open, int nExpectedNofRows,  int nExpectedNofCols, BOOL bCopyToEnergyArray, int nLeadInOut, int nGONIO_COL_IDX, int nNoofRowsBetweenBeams){
 FILE *open_file;
 char BigBuff[72002];//Assume Maximum number of chars in a  line is 2000 * Max length of a field + 1999 commas + cr/lf + one char to allow for the null terminator. 
                     //Add 1 more char for good measure, but object if fgets() is able to read something into it, as this indicates that the above assumption is incorrect.
                     //I'll assume 30 decimal places + the point + 4 significant places i.e (2000 * 35) + 2000 + 3
 errno_t err;
 int nExpectedNofColsTimes2 = nExpectedNofCols * 2;
 int nCol;
 int returncode = 0;
 char *stopstring;
 err = fopen_s(&open_file, pszFile2Open, "rt");
 if( err ){
    myLoadString(STR_1); //Unable to open and read file...\n\n"
    StringCchCat(mes,SIZMES,pszFile2Open);
    MessageBox (NULL,mes,pszFile2Open,MB_OK);
    returncode = 1;
   }
 else{
    char *pToken = NULL;
    char *pNextToken = NULL;
    int lenBigBuffString;
#if 0 //This version expects the Spreadsheet to Reverse alternate rows
    for(int rowNo = 0; rowNo < nExpectedNofRows; rowNo++){
       fgets(BigBuff,72002,open_file); //Note that as the file is opened in text mode carriage returns are stripped off, but the line feeds are still present
       lenBigBuffString = lstrlen(BigBuff);
       if(lenBigBuffString < nExpectedNofColsTimes2){
          sprintf_s(mes,SIZMES,"Error: The above-named file contained a line with fewer than 'nExpectedNofColsTimes2' characters in!\nThe Error occurred on line number: %d",rowNo+1);
          MessageBox (NULL, mes, pszFile2Open, MB_OK);
          returncode = 2;
          rowNo = nExpectedNofRows;
         }
       else if(lenBigBuffString > 72000){
          sprintf_s(mes,SIZMES,"Error: The above-named file contained a line with over 72000 characters in!\nThe Error occurred on line number: %d",rowNo+1);
          MessageBox (NULL, mes, pszFile2Open, MB_OK);
          returncode = 3;
          rowNo = nExpectedNofRows;
         }
       else{
          pToken = NULL;
          pNextToken = NULL;
          pToken= strtok_s(BigBuff, ",", &pNextToken);//" ,.-"
          nCol = 0;
          while (pToken != NULL){
             StringCchCopy(mes, SIZMES, pToken);
             if(nCol < nExpectedNofCols) //prevent array going out of bounds
               ldblArrayOfGonioAngles[rowNo][nCol++] = strtod( mes, &stopstring);
             pToken= strtok_s(NULL, " ,", &pNextToken);//",.-"
            }
          if(nCol != nExpectedNofCols){
              sprintf_s(mes,SIZMES,"Error: The above-named file contained a line that did not have the right number of columns in!\nThe Error occurred on line number: %d\nExpected Columns: %d  Received Columns: %d",rowNo+1, nExpectedNofCols, nCol);
             MessageBox (NULL, mes, pszFile2Open, MB_OK);
             returncode = 4;
             rowNo = nExpectedNofRows;
            }
         }
      }
#else //and this version doesn't expect the Spreadsheet to Reverse alternate rows
    BOOL bEvenRow = 0;
    int colDecrementing;
    for(int rowNo = 0; rowNo < nExpectedNofRows; rowNo++){
       bEvenRow = !bEvenRow;
       fgets(BigBuff,72002,open_file); //Note that as the file is opened in text mode carriage returns are stripped off, but the line feeds are still present
       lenBigBuffString = lstrlen(BigBuff);
       if(lenBigBuffString < nExpectedNofColsTimes2){
          sprintf_s(mes,SIZMES,"Error: The above-named file contained a line with fewer than 'nExpectedNofColsTimes2' characters in!\nThe Error occurred on line number: %d",rowNo+1);
          MessageBox (NULL, mes, pszFile2Open, MB_OK);
          returncode = 2;
          rowNo = nExpectedNofRows;
         }
       else if(lenBigBuffString > 72000){
          sprintf_s(mes,SIZMES,"Error: The above-named file contained a line with over 72000 characters in!\nThe Error occurred on line number: %d",rowNo+1);
          MessageBox (NULL, mes, pszFile2Open, MB_OK);
          returncode = 3;
          rowNo = nExpectedNofRows;
         }
       else{
          pToken = NULL;
          pNextToken = NULL;
          pToken= strtok_s(BigBuff, ",", &pNextToken);//" ,.-"
          nCol = 0;
          if(bEvenRow)
            while (pToken != NULL){
               StringCchCopy(mes, SIZMES, pToken);
               ldblArrayOfGonioAngles[rowNo][nCol++] = strtod( mes, &stopstring);
               pToken= strtok_s(NULL, " ,", &pNextToken);//",.-"
              }
          else{
             colDecrementing = nExpectedNofCols - 1;
             while (pToken != NULL){
                nCol++;
                StringCchCopy(mes, SIZMES, pToken);
                if(colDecrementing >= 0)//prevent array going out of bounds
                  ldblArrayOfGonioAngles[rowNo][colDecrementing--] = strtod( mes, &stopstring);
                pToken= strtok_s(NULL, " ,", &pNextToken);//",.-"
               }
            }
          if(nCol != nExpectedNofCols){
              sprintf_s(mes,SIZMES,"Error: The above-named file contained a line that did not have the right number of columns in!\nThe Error occurred on line number: %d\nExpected Columns: %d  Received Columns: %d",rowNo+1, nExpectedNofCols, nCol);
             MessageBox (NULL, mes, pszFile2Open, MB_OK);
             returncode = 4;
             rowNo = nExpectedNofRows;
            }
         }
      }
#endif
    if(!returncode){//We should have read the whole file, but as a final check that something is not wrong, try to read another string.  If anything is read, object - telling user the file is too long.
       BigBuff[0] = 0;
       fgets(BigBuff,72002,open_file); //Note that as the file is opened in text mode carriage returns are stripped off, but the line feeds are still present
       pToken = NULL;
       pNextToken = NULL;
       pToken= strtok_s(BigBuff, ",", &pNextToken);//" ,.-"
       if(pToken != NULL){
          sprintf_s(mes,SIZMES,"Error: The above-named file contained data after the expected End of file at line number %d",nExpectedNofRows);
          MessageBox (NULL, mes, pszFile2Open, MB_OK);
          returncode = 5;
         }
       if(!returncode && bCopyToEnergyArray){
//I think I need the equivalent of nGONIO_COL_IDX here, to allow for the distance between the rows (see below the next line)
// Also, I'll use values 3,4 & 5 (instead of 1,2 & 3) to differentiate between M2(rgb) and M3(rgb)
          int nCol;
          int nOffsetCol;
          int nLeadInCol;
          int nLeadOutCol;
          int nTotalCols;
          double dbkFirstOrLastVal;
          if(nGONIO_COL_IDX == 0){
           ////memcpy(dblArDesiredEnergyWP3r,ldblArrayOfGonioAngles,nExpectedNofRows*nExpectedNofCols*sizeof(double));  //This commented out block expected the coordinates from Altera to be in range
           //  for(int nRow = 0; nRow < nExpectedNofRows; nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP3r[nRow][nCol] = ldblArrayOfGonioAngles[nRow][nCol];
           //  for(int nRow = nExpectedNofRows; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP3r[nRow][nCol] = dblArDesiredEnergyWP3r[nExpectedNofRows - 1][nExpectedNofCols - 1];
             for(int nRow = 0; nRow < nExpectedNofRows; nRow++){
                nOffsetCol = nLeadInOut;
                for(nCol = 0; nCol < nExpectedNofCols; nCol++)
                  dblArDesiredEnergyWP3r[nRow][nOffsetCol++] = ldblArrayOfGonioAngles[nRow][nCol]; //This puts the data from the ExcelFiles into the proper position (so we don't need to adjust the coords from Altera)
                dbkFirstOrLastVal = dblArDesiredEnergyWP3r[nRow][nLeadInOut];  //Now Ensure the data during this line's LeadIn
                for(nLeadInCol = 0; nLeadInCol < nLeadInOut; nLeadInCol++) // is all set to the first value
                  dblArDesiredEnergyWP3r[nRow][nLeadInCol] = dbkFirstOrLastVal;//  to be used when the shutter's open
               }
           //Now, the values during the LeadOut for every line should be set to the start position for the next line (for every line APART FROM THE LAST LINE)
             int nExpectedNofRowsM1 = nExpectedNofRows - 1;
             nTotalCols = nExpectedNofCols + 2 * nLeadInOut;
             for(int nRow = 0; nRow < nExpectedNofRowsM1; nRow++){
                dbkFirstOrLastVal = dblArDesiredEnergyWP3r[nRow + 1][0]; //This is the first value of next row
                for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
                  dblArDesiredEnergyWP3r[nRow][nLeadOutCol] = dbkFirstOrLastVal;
               }
           //For Completeness, I'll set the leadout for the last row back to the start position for the next print (although we don't much care what the PID does after it's finished)
             for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
               dblArDesiredEnergyWP3r[nExpectedNofRowsM1][nLeadOutCol] = dblArDesiredEnergyWP3r[0][0];
           //For Red, the last value in the excel file must be copied to all positions for an extra "nNoofRowsBetweenBeams * 2" rows
             for(int nRow = nExpectedNofRows; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP3r[nRow][nCol] = dblArDesiredEnergyWP3r[0][0];
            }
          else if (nGONIO_COL_IDX == 1){
           ////memcpy((char*)dblArDesiredEnergyWP3g+(nNoofRowsBetweenBeams*nExpectedNofCols*sizeof(double)), ldblArrayOfGonioAngles, nExpectedNofRows*nExpectedNofCols*sizeof(double));
           //  for(int nRow = 0; nRow < nExpectedNofRows; nRow++)   //This commented out block expected the coordinates from Altera to be in range
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP3g[nRow+nNoofRowsBetweenBeams][nCol] = ldblArrayOfGonioAngles[nRow][nCol];
           //  for(int nRow = 0; nRow < nNoofRowsBetweenBeams; nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP3g[nRow][nCol] = dblArDesiredEnergyWP3g[nNoofRowsBetweenBeams][0];
           //  for(int nRow = nExpectedNofRows + nNoofRowsBetweenBeams; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP3g[nRow][nCol] = dblArDesiredEnergyWP3g[nExpectedNofRows + nNoofRowsBetweenBeams - 1][nExpectedNofCols - 1];
             for(int nRow = 0; nRow < nExpectedNofRows; nRow++){
                nOffsetCol = nLeadInOut;
                for(nCol = 0; nCol < nExpectedNofCols; nCol++)
                  dblArDesiredEnergyWP3g[nRow+nNoofRowsBetweenBeams][nOffsetCol++] = ldblArrayOfGonioAngles[nRow][nCol]; //This puts the data from the ExcelFiles into the proper position (so we don't need to adjust the coords from Altera)
                dbkFirstOrLastVal = dblArDesiredEnergyWP3g[nRow+nNoofRowsBetweenBeams][nLeadInOut];  //Now Ensure the data during this line's LeadIn
                for(nLeadInCol = 0; nLeadInCol < nLeadInOut; nLeadInCol++)                           // is all set to the first value
                  dblArDesiredEnergyWP3g[nRow+nNoofRowsBetweenBeams][nLeadInCol] = dbkFirstOrLastVal;//  to be used when the shutter's open
               }
           //Now, the values during the LeadOut for every line should be set to the start position for the next line (for every line APART FROM THE LAST LINE)
             int nExpectedNofRowsM1 = nExpectedNofRows - 1;
             nTotalCols = nExpectedNofCols + 2 * nLeadInOut;
             for(int nRow = 0; nRow < nExpectedNofRowsM1; nRow++){
                dbkFirstOrLastVal = dblArDesiredEnergyWP3g[nRow + nNoofRowsBetweenBeams + 1][0]; //This is the first value of next row
                for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
                  dblArDesiredEnergyWP3g[nRow + nNoofRowsBetweenBeams][nLeadOutCol] = dbkFirstOrLastVal;
               }
           //For Completeness, I'll set the leadout for the last row back to the start position for the next print (although we don't much care what the PID does after it's finished)
             for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
               dblArDesiredEnergyWP3g[nExpectedNofRowsM1+nNoofRowsBetweenBeams][nLeadOutCol] = dblArDesiredEnergyWP3g[nNoofRowsBetweenBeams][0];
           //For Gre, the last value in the excel file must be copied to all positions for an extra "nNoofRowsBetweenBeams" rows
             for(int nRow = nExpectedNofRows + nNoofRowsBetweenBeams; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP3g[nRow][nCol] = dblArDesiredEnergyWP3g[nNoofRowsBetweenBeams][0];
           //... and also to the extra "nNoofRowsBetweenBeams" rows occuring at the start
             for(int nRow = 0; nRow < nNoofRowsBetweenBeams; nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP3g[nRow][nCol] = dblArDesiredEnergyWP3g[nNoofRowsBetweenBeams][0];
            }
          else if (nGONIO_COL_IDX == 2){
           ////memcpy((char*)dblArDesiredEnergyWP3b+(nNoofRowsBetweenBeams * 2 * nExpectedNofCols * sizeof(double)), ldblArrayOfGonioAngles, nExpectedNofRows*nExpectedNofCols*sizeof(double));
           //  for(int nRow = 0; nRow < nExpectedNofRows; nRow++)   //This commented out block expected the coordinates from Altera to be in range
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP3b[nRow+nNoofRowsBetweenBeams*2][nCol] = ldblArrayOfGonioAngles[nRow][nCol];
           //  for(int nRow = 0; nRow < (nNoofRowsBetweenBeams * 2); nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP3b[nRow][nCol] = dblArDesiredEnergyWP3b[nNoofRowsBetweenBeams * 2][0];
             int nNoofRowsBetweenBeamsTimes2 = nNoofRowsBetweenBeams * 2;
             int nNoofRowsBetweenBeamsTimes2plus1 = nNoofRowsBetweenBeamsTimes2 + 1;
             for(int nRow = 0; nRow < nExpectedNofRows; nRow++){
                nOffsetCol = nLeadInOut;
                for(nCol = 0; nCol < nExpectedNofCols; nCol++)
                  dblArDesiredEnergyWP3b[nRow+nNoofRowsBetweenBeamsTimes2][nOffsetCol++] = ldblArrayOfGonioAngles[nRow][nCol]; //This puts the data from the ExcelFiles into the proper position (so we don't need to adjust the coords from Altera)
                dbkFirstOrLastVal = dblArDesiredEnergyWP3b[nRow+nNoofRowsBetweenBeamsTimes2][nLeadInOut];  //Now Ensure the data during this line's LeadIn
                for(nLeadInCol = 0; nLeadInCol < nLeadInOut; nLeadInCol++)                                 // is all set to the first value
                  dblArDesiredEnergyWP3b[nRow+nNoofRowsBetweenBeamsTimes2][nLeadInCol] = dbkFirstOrLastVal;//  to be used when the shutter's open
               }
           //Now, the values during the LeadOut for every line should be set to the start position for the next line (for every line APART FROM THE LAST LINE)
             int nExpectedNofRowsM1 = nExpectedNofRows - 1;
             nTotalCols = nExpectedNofCols + 2 * nLeadInOut;
             for(int nRow = 0; nRow < nExpectedNofRowsM1; nRow++){
                dbkFirstOrLastVal = dblArDesiredEnergyWP3b[nRow + nNoofRowsBetweenBeamsTimes2plus1][0]; //This is the first value of next row
                for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
                  dblArDesiredEnergyWP3b[nRow + nNoofRowsBetweenBeamsTimes2][nLeadOutCol] = dbkFirstOrLastVal;
               }
           //For Completeness, I'll set the leadout for the last row back to the start position for the next print (although we don't much care what the PID does after it's finished)
             for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
               dblArDesiredEnergyWP3b[nExpectedNofRowsM1+nNoofRowsBetweenBeamsTimes2][nLeadOutCol] = dblArDesiredEnergyWP3b[nNoofRowsBetweenBeamsTimes2][0];
           //For Blu, the last value in the excel file must be copied to all positions for an extra "nNoofRowsBetweenBeams *2 " rows occuring at the start
             for(int nRow = 0; nRow < nNoofRowsBetweenBeamsTimes2; nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP3b[nRow][nCol] = dblArDesiredEnergyWP3b[nNoofRowsBetweenBeamsTimes2][0];
            }

          if(nGONIO_COL_IDX == 3){
           ////memcpy(dblArDesiredEnergyWP2r,ldblArrayOfGonioAngles,nExpectedNofRows*nExpectedNofCols*sizeof(double));  //This commented out block expected the coordinates from Altera to be in range
           //  for(int nRow = 0; nRow < nExpectedNofRows; nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP2r[nRow][nCol] = ldblArrayOfGonioAngles[nRow][nCol];
           //  for(int nRow = nExpectedNofRows; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP2r[nRow][nCol] = dblArDesiredEnergyWP2r[nExpectedNofRows - 1][nExpectedNofCols - 1];
             for(int nRow = 0; nRow < nExpectedNofRows; nRow++){
                nOffsetCol = nLeadInOut;
                for(nCol = 0; nCol < nExpectedNofCols; nCol++)
                  dblArDesiredEnergyWP2r[nRow][nOffsetCol++] = ldblArrayOfGonioAngles[nRow][nCol]; //This puts the data from the ExcelFiles into the proper position (so we don't need to adjust the coords from Altera)
                dbkFirstOrLastVal = dblArDesiredEnergyWP2r[nRow][nLeadInOut];  //Now Ensure the data during this line's LeadIn
                for(nLeadInCol = 0; nLeadInCol < nLeadInOut; nLeadInCol++) // is all set to the first value
                  dblArDesiredEnergyWP2r[nRow][nLeadInCol] = dbkFirstOrLastVal;//  to be used when the shutter's open
               }
           //Now, the values during the LeadOut for every line should be set to the start position for the next line (for every line APART FROM THE LAST LINE)
             int nExpectedNofRowsM1 = nExpectedNofRows - 1;
             nTotalCols = nExpectedNofCols + 2 * nLeadInOut;
             for(int nRow = 0; nRow < nExpectedNofRowsM1; nRow++){
                dbkFirstOrLastVal = dblArDesiredEnergyWP2r[nRow + 1][0]; //This is the first value of next row
                for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
                  dblArDesiredEnergyWP2r[nRow][nLeadOutCol] = dbkFirstOrLastVal;
               }
           //For Completeness, I'll set the leadout for the last row back to the start position for the next print (although we don't much care what the PID does after it's finished)
             for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
               dblArDesiredEnergyWP2r[nExpectedNofRowsM1][nLeadOutCol] = dblArDesiredEnergyWP2r[0][0];
           //For Red, the last value in the excel file must be copied to all positions for an extra "nNoofRowsBetweenBeams * 2" rows
             for(int nRow = nExpectedNofRows; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP2r[nRow][nCol] = dblArDesiredEnergyWP2r[0][0];
            }
          else if (nGONIO_COL_IDX == 4){
           ////memcpy((char*)dblArDesiredEnergyWP2g+(nNoofRowsBetweenBeams*nExpectedNofCols*sizeof(double)), ldblArrayOfGonioAngles, nExpectedNofRows*nExpectedNofCols*sizeof(double));
           //  for(int nRow = 0; nRow < nExpectedNofRows; nRow++)   //This commented out block expected the coordinates from Altera to be in range
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP2g[nRow+nNoofRowsBetweenBeams][nCol] = ldblArrayOfGonioAngles[nRow][nCol];
           //  for(int nRow = 0; nRow < nNoofRowsBetweenBeams; nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP2g[nRow][nCol] = dblArDesiredEnergyWP2g[nNoofRowsBetweenBeams][0];
           //  for(int nRow = nExpectedNofRows + nNoofRowsBetweenBeams; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP2g[nRow][nCol] = dblArDesiredEnergyWP2g[nExpectedNofRows + nNoofRowsBetweenBeams - 1][nExpectedNofCols - 1];
             for(int nRow = 0; nRow < nExpectedNofRows; nRow++){
                nOffsetCol = nLeadInOut;
                for(nCol = 0; nCol < nExpectedNofCols; nCol++)
                  dblArDesiredEnergyWP2g[nRow+nNoofRowsBetweenBeams][nOffsetCol++] = ldblArrayOfGonioAngles[nRow][nCol]; //This puts the data from the ExcelFiles into the proper position (so we don't need to adjust the coords from Altera)
                dbkFirstOrLastVal = dblArDesiredEnergyWP2g[nRow+nNoofRowsBetweenBeams][nLeadInOut];  //Now Ensure the data during this line's LeadIn
                for(nLeadInCol = 0; nLeadInCol < nLeadInOut; nLeadInCol++)                           // is all set to the first value
                  dblArDesiredEnergyWP2g[nRow+nNoofRowsBetweenBeams][nLeadInCol] = dbkFirstOrLastVal;//  to be used when the shutter's open
               }
           //Now, the values during the LeadOut for every line should be set to the start position for the next line (for every line APART FROM THE LAST LINE)
             int nExpectedNofRowsM1 = nExpectedNofRows - 1;
             nTotalCols = nExpectedNofCols + 2 * nLeadInOut;
             for(int nRow = 0; nRow < nExpectedNofRowsM1; nRow++){
                dbkFirstOrLastVal = dblArDesiredEnergyWP2g[nRow + nNoofRowsBetweenBeams + 1][0]; //This is the first value of next row
                for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
                  dblArDesiredEnergyWP2g[nRow + nNoofRowsBetweenBeams][nLeadOutCol] = dbkFirstOrLastVal;
               }
           //For Completeness, I'll set the leadout for the last row back to the start position for the next print (although we don't much care what the PID does after it's finished)
             for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
               dblArDesiredEnergyWP2g[nExpectedNofRowsM1+nNoofRowsBetweenBeams][nLeadOutCol] = dblArDesiredEnergyWP2g[nNoofRowsBetweenBeams][0];
           //For Gre, the last value in the excel file must be copied to all positions for an extra "nNoofRowsBetweenBeams" rows
             for(int nRow = nExpectedNofRows + nNoofRowsBetweenBeams; nRow < (nNoofRowsBetweenBeams * 2 + nExpectedNofRows); nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP2g[nRow][nCol] = dblArDesiredEnergyWP2g[nNoofRowsBetweenBeams][0];
           //... and also to the extra "nNoofRowsBetweenBeams" rows occuring at the start
             for(int nRow = 0; nRow < nNoofRowsBetweenBeams; nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP2g[nRow][nCol] = dblArDesiredEnergyWP2g[nNoofRowsBetweenBeams][0];
            }
          else if (nGONIO_COL_IDX == 5){
           ////memcpy((char*)dblArDesiredEnergyWP2b+(nNoofRowsBetweenBeams * 2 * nExpectedNofCols * sizeof(double)), ldblArrayOfGonioAngles, nExpectedNofRows*nExpectedNofCols*sizeof(double));
           //  for(int nRow = 0; nRow < nExpectedNofRows; nRow++)   //This commented out block expected the coordinates from Altera to be in range
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP2b[nRow+nNoofRowsBetweenBeams*2][nCol] = ldblArrayOfGonioAngles[nRow][nCol];
           //  for(int nRow = 0; nRow < (nNoofRowsBetweenBeams * 2); nRow++)
           //    for(int nCol = 0; nCol < nExpectedNofCols; nCol++)
           //      dblArDesiredEnergyWP2b[nRow][nCol] = dblArDesiredEnergyWP2b[nNoofRowsBetweenBeams * 2][0];
             int nNoofRowsBetweenBeamsTimes2 = nNoofRowsBetweenBeams * 2;
             int nNoofRowsBetweenBeamsTimes2plus1 = nNoofRowsBetweenBeamsTimes2 + 1;
             for(int nRow = 0; nRow < nExpectedNofRows; nRow++){
                nOffsetCol = nLeadInOut;
                for(nCol = 0; nCol < nExpectedNofCols; nCol++)
                  dblArDesiredEnergyWP2b[nRow+nNoofRowsBetweenBeamsTimes2][nOffsetCol++] = ldblArrayOfGonioAngles[nRow][nCol]; //This puts the data from the ExcelFiles into the proper position (so we don't need to adjust the coords from Altera)
                dbkFirstOrLastVal = dblArDesiredEnergyWP2b[nRow+nNoofRowsBetweenBeamsTimes2][nLeadInOut];  //Now Ensure the data during this line's LeadIn
                for(nLeadInCol = 0; nLeadInCol < nLeadInOut; nLeadInCol++)                                 // is all set to the first value
                  dblArDesiredEnergyWP2b[nRow+nNoofRowsBetweenBeamsTimes2][nLeadInCol] = dbkFirstOrLastVal;//  to be used when the shutter's open
               }
           //Now, the values during the LeadOut for every line should be set to the start position for the next line (for every line APART FROM THE LAST LINE)
             int nExpectedNofRowsM1 = nExpectedNofRows - 1;
             nTotalCols = nExpectedNofCols + 2 * nLeadInOut;
             for(int nRow = 0; nRow < nExpectedNofRowsM1; nRow++){
                dbkFirstOrLastVal = dblArDesiredEnergyWP2b[nRow + nNoofRowsBetweenBeamsTimes2plus1][0]; //This is the first value of next row
                for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
                  dblArDesiredEnergyWP2b[nRow + nNoofRowsBetweenBeamsTimes2][nLeadOutCol] = dbkFirstOrLastVal;
               }
           //For Completeness, I'll set the leadout for the last row back to the start position for the next print (although we don't much care what the PID does after it's finished)
             for(nLeadOutCol = nExpectedNofCols + nLeadInOut; nLeadOutCol < nTotalCols; nLeadOutCol++)
               dblArDesiredEnergyWP2b[nExpectedNofRowsM1+nNoofRowsBetweenBeamsTimes2][nLeadOutCol] = dblArDesiredEnergyWP2b[nNoofRowsBetweenBeamsTimes2][0];
           //For Blu, the last value in the excel file must be copied to all positions for an extra "nNoofRowsBetweenBeams *2 " rows occuring at the start
             for(int nRow = 0; nRow < nNoofRowsBetweenBeamsTimes2; nRow++)
               for(nCol = 0; nCol < nTotalCols; nCol++)
                 dblArDesiredEnergyWP2b[nRow][nCol] = dblArDesiredEnergyWP2b[nNoofRowsBetweenBeamsTimes2][0];
            }
          //I may need also to read the file in to the program early on, to give the 
         }
      }
    fclose ( open_file );
   }
 return returncode;
}

//double GetWyInMMFromSpreadSheetExportFile(int nHorzCoord, int nVertCoord){//Coords in pixels
// double dblColumnS;
// dblColumnS = nHorzCoord + nVertCoord;
// return dblColumnS;    //This is column S
//}

//double GetWxInMMFromSpreadSheetExportFile(int nHorzCoord, int nVertCoord){//Coords in pixels
// double dblColumnR;
// dblColumnR = nHorzCoord + nVertCoord;
// return dblColumnR;    //This is column R
//}
