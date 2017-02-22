using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

using DevExpress.UserSkins;
using DevExpress.Skins;
using DevExpress.LookAndFeel;
using DevExpress.XtraSplashScreen;
using System.Threading;
using DevExpress.XtraEditors;

namespace DomQuaChk
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            BonusSkins.Register();
            SkinManager.EnableFormSkins();
            UserLookAndFeel.Default.SetSkinStyle("DevExpress Style");

            Application.Run(new DomQuaChk());
        }
    }
}
