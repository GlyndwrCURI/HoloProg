// DO NOT DELETE THIS FILE    DO NOT DELETE THIS FILE    DO NOT DELETE THIS FILE    DO NOT DELETE THIS FILE    DO NOT DELETE THIS FILE    DO NOT DELETE THIS FILE    DO NOT DELETE THIS FILE
//This file contains the default data for the default PCFRECORD 'DefaultPCFrec'.  The default HCF record also #includes this file, so it gets a copy of this data too.
//This is probably unnecessary, as the data itself will be overwritten when a hologram is generated, but at least the default data needs maintaining in one place only
 //0,0,0,0,//nM39_XAXISBACKLASH
//32,16,32,16,32,16,32,16,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  //nUnused[199];
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0, //nM39_XAXISBACKLASH
 "192.168.000.201","192.168.000.202","192.168.000.203","192.168.000.204","192.168.000.205","192.168.000.206","192.168.000.207","192.168.000.208","192.168.000.209","192.168.000.210",
 "192.168.000.211","192.168.000.212","192.168.000.213","192.168.000.214","192.168.000.215","192.168.000.216","192.168.000.217","192.168.000.218","192.168.000.219","192.168.000.220",
 "192.168.000.221","192.168.000.222","192.168.000.223","192.168.000.224","192.168.000.225","192.168.000.226","192.168.000.227","192.168.000.228","192.168.000.229","192.168.000.230",
 "192.168.000.231","192.168.000.232","192.168.000.233","192.168.000.234","192.168.000.235","192.168.000.236","192.168.000.237","192.168.000.238","192.168.000.239","192.168.000.240",
 32,32,32,32,32,32,32,32,32,32,32,32,
 32,32,32,32,32,32,32,32,32,32,32,32,
 32,32,32,32,32,32,32,32,32,32,32,32,32,32,4,2,
 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,   //Temporarily expecting only M2, M4 and M39 to reply
 //1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1, //These are the motors we expect replies from
 0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,   //These are bMotorIS_A_PID
 2133,133,133,        //WP1MinEnergy
 233,233,233,         //WP2MinEnergy
 333,334,335,         //WP3MinEnergy
 1008,1008,1008,      //LOobjXXXmm
 3200,6400,76800,3200,6400,76800,3200,6400,76800,  //Usteps4EachApertureTab
 3200,3300,3400,      //FocusTab
 7,7,7,     //Hm2CenterPointT
 8,8,8,     //Hm2CenterPointB
 9,9,9,     //Hm2CenterPointL
 5,5,5,     //Hm2CenterPointR
 5,5,5,     //Hm2GonHzeroDegrees
 0,0,0,     //Hm2GonVzeroDegrees
 3700,3800,3373,3838, //RefBeamTablePrintUnload
 1000,0,   //Hm2 point (0,0) XY Stage
 //The Next Two Rows are Physical Limits of Travel (nPhysLimMinUsteps[Mx] & nPhysLimMaxUsteps[Mx). They may not be implemented for every motor (e.g. PIDs) but all motors are included for good measure.
 // They should be set by the operator or installation engineer and changed whenever uSteps / Step changes.  Procedure for setting them up is Home motor, the drive stage into the Min and
 // Max positions (e.g. Limit switches), then back off a little.  The Physical limits are checked by the software when generating the motor positions... in particular when acceleration
 // profiles are calculated the distance required by the acceleration can be subracted from, or added to, the start or end points of printing.  Provided these are within the physical
 // limits of travel the profiles generated should be possible.
 -2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,
 -2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,
 -2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,
 -2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-2147483647,-1000,-7200,
 
  2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647,
  2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647,
  2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647,
  2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 1200000, 8640000