using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

using System.Runtime.InteropServices;
namespace ControlBoxWinSoft
{

[Guid("5C28AED5-F911-4b33-89A4-B99D7335F14A")] //[ComVisible(true)]       //[interfaceTypeAttribute(ComInterfaceTypw.InterfaceIsDual)]
public interface ImyDotNetInterface {
    [DispId(1)]
    int myInitialize(int anArg);
   }

 [Guid("26C8CF50-6E24-4689-816A-7D70E6A1305A")]   //[ComVisible(true)] //[ProgId("My.Prog.ID")]
 [ClassInterface(ClassInterfaceType.AutoDual)]    // .AutoDispatch  .None
 public class Program : ImyDotNetInterface {  // In registry, the above guid appears to be associated with class rrppProgram and My.Prog.ID

     public int myInitialize(int anArg)
     {
       //if (anArg == 6)
       //  return 67;
       string anArgAsString;
       string[] stringArray = new string[1];

       anArgAsString = anArg.ToString();
       stringArray[0] = anArgAsString;
       anArg = Main(stringArray);

       if(anArg == 6)
         return 4;
       else
         return 8;
      }

     static int Main(string[] args)
     {
       Int32 asInt;
       //asInt = args[0].ToInt32();
       int nSiz = args.Length;
       if (nSiz > 0)
         asInt = Int32.Parse(args[0]);
       else
         asInt = 7;
       Application.EnableVisualStyles();
       //Application.SetCompatibleTextRenderingDefault(false); // < - Disasterous!!
       Application.Run(new Form1());
       return asInt;
      }
   }
}
