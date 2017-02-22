/*********************************************************************
 *                 版权: 武汉大学遥感信息工程学院DPGRID小组
 *                 作者: 谭凯
 *                 功能: 基于灰度相关性的面向对象云检测界面程序
 *                 创建时间: 2017.02.13
 *********************************************************************/
using System;
using System.Runtime.InteropServices;

using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;
using System.Diagnostics;
using DevExpress.XtraEditors;

using TEDS_App; 

namespace DomQuaChk
{
    public partial class DomQuaChk : DevExpress.XtraEditors.XtraForm
    {
        public enum FileType
        {
            PRO = 0,
            DOM = 1,
            REF = 2,
        }
        private List<RowDataRecord> studentList;    //界面中绑定的数据源
        private DirectoryInfo flod_temp_vaild_files;
        private Process process_get_tasks;
        public DomQuaChk()
        {
            InitializeComponent();
            InitialDialog();
        }
        //@初始化对话框中控件状态        
        public void InitialDialog()
        {
            this.check_edit_tif_.Checked = true;
            this.check_edit_pix_.Checked = true;
            this.check_edit_add_.Checked = true;
            this.check_edit_run_gcsvr_.Checked = true;
            this.check_edit_run_gcnode_.Checked = true;
            this.check_edit_position_.Checked = true;
            this.check_edit_matching_.Checked = true;

            this.splitContainer2.Panel2Collapsed = true;

            studentList = new List<RowDataRecord>();
            //绑定！
            this.grid_control_.DataSource = studentList;
            this.grid_control_.RefreshDataSource();

        }
        //@启动进程创建tsk任务
        //!exe_path 要启动的程序路径
        //!str_arguments 命令行参数
        private Process CreateProcessGetTasks(string exe_path, string str_arguments)
        {
            Process process = new System.Diagnostics.Process();
            process.StartInfo.FileName = exe_path;
            process.StartInfo.Arguments = str_arguments;
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.RedirectStandardInput = true;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.RedirectStandardError = true;
            process.StartInfo.CreateNoWindow = true;
            process.Start();
            return process;
        }
        //@刷新检查各类影像数量
        private void CheckImagesNumber()
        {
            int n_dom_images_number = 0;
            int n_ref_images_number = 0;
            foreach(RowDataRecord record in studentList)
            {
                if (record.str_type_ == "DOM") n_dom_images_number++;
                if (record.str_type_ == "REF") n_ref_images_number++;
            }
            this.text_edit_dom_number_.Text = n_dom_images_number.ToString();
            this.text_edit_ref_number_.Text = n_ref_images_number.ToString();
        }
        //@载入XML工程文件中的影像数据
        private void LoadXmlData(string str_xml_path)
        {
            if (!check_edit_add_.Checked) studentList.Clear();

            FileInfo file_infor = new FileInfo(str_xml_path);
            if (!file_infor.Exists)
            {
                this.grid_control_.RefreshDataSource();
                CheckImagesNumber();
                return;
            }
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(str_xml_path);

            XmlNodeList image_lists = xmlDoc.SelectNodes("ImageList");
            XmlNodeList images;
            XmlNode     image_type;
            XmlNode     image_path;
            foreach(XmlNode root in image_lists)
            {
                images = root.SelectNodes("Image");
                foreach(XmlNode image in images)
                {
                    image_type = image.SelectSingleNode("WorkType");
                    image_path = image.SelectSingleNode("Path");
                    if (image_type.InnerText == "DOM" && image_path.InnerText != null) studentList.Add(new RowDataRecord(studentList.Count, image_path.InnerText, "DOM"));
                    else if (image_type.InnerText == "REF" && image_path.InnerText != null) studentList.Add(new RowDataRecord(studentList.Count, image_path.InnerText, "REF"));
                }
            }
            this.grid_control_.RefreshDataSource();
            CheckImagesNumber();
        }
        //@将界面中的数据源写入XML工程文件
        private bool SaveXmlData(string str_xml_path)
        {
            if (str_xml_path == "")
            {
                XtraMessageBox.Show("工程文件路径无效!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            XmlDocument xmlDoc = new XmlDocument();
            XmlDeclaration xml_declaration = xmlDoc.CreateXmlDeclaration("1.0", "gb2312", "yes");

            XmlNode image_list = xmlDoc.CreateElement("ImageList");//创建一个﹤ImageList﹥节点 
            foreach(RowDataRecord record in studentList)
            {
                XmlElement image = xmlDoc.CreateElement("Image");   //创建一个﹤Image﹥节点 
                XmlElement id = xmlDoc.CreateElement("ID");         //创建一个﹤ID﹥节点 
                XmlElement path = xmlDoc.CreateElement("Path");     //创建一个﹤Path﹥节点 
                XmlElement type = xmlDoc.CreateElement("WorkType"); //创建一个﹤WorkType﹥节点 

                id.InnerText = record.n_id_.ToString();
                path.InnerText = record.str_path_;
                type.InnerText = record.str_type_;
                image.AppendChild(id);
                image.AppendChild(path);
                image.AppendChild(type);

                image_list.AppendChild(image);
            }
            xmlDoc.AppendChild(xml_declaration);
            xmlDoc.AppendChild(image_list);
            xmlDoc.Save(str_xml_path);
            return true;
        }
        //@获取指定文件夹中指定格式的文件
        private void GetFilesFromFold(DirectoryInfo directory,string pattern)
        {
            if (!check_edit_add_.Checked) studentList.Clear();

            int n_student = studentList.Count;
            if (directory.Exists || pattern.Trim() != string.Empty) 
            {
                foreach(FileInfo info in directory.GetFiles())
                {
                    if (this.check_edit_tif_.Checked && (info.Name.ToLower().EndsWith(".tif") || info.Name.ToLower().EndsWith(".tiff"))) studentList.Add(new RowDataRecord(studentList.Count, info.FullName, pattern));
                    else if (this.check_edit_pix_.Checked && info.Name.ToLower() == ".pix") studentList.Add(new RowDataRecord(studentList.Count, info.FullName, pattern));
                }
                foreach(DirectoryInfo info_cir in directory.GetDirectories())
                {
                    GetFilesFromFold(info_cir, pattern);
                }
            }
            this.grid_control_.RefreshDataSource();
            CheckImagesNumber();
        }
        //@打开保存文件对话框，设置路径        
        private string SelectFilePath(string str, FileType type)
        {
            //OpenFileDialog fp = new OpenFileDialog();
            SaveFileDialog fp = new SaveFileDialog();
            fp.Title = str;
            fp.CreatePrompt = false;
            fp.OverwritePrompt = false;
            //fp.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Templates);  //指定默认路径
            if (type == FileType.DOM || type == FileType.REF)
            {
                if (this.check_edit_tif_.Checked) fp.Filter += "Tif File(*.tif)|*.tif|Tiff File(*.tiff)|*.tiff|";
                if (this.check_edit_pix_.Checked) fp.Filter += "Pix File(*.pix)|*.pix|";
                fp.Filter += "ALL File(*.*)|*.*";
            }
            else if (type == FileType.PRO) fp.Filter = "Xml File(*.xml)|*.xml|ALL File(*.*)|*.*";
            else return "error";
            
            if (fp.ShowDialog() == DialogResult.OK) return fp.FileName;
            else
            {
                XtraMessageBox.Show("文件路径获取失败!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return null;
            }
        }
        //@设置工程路径        
        private void SelectProjectPath(object sender, DevExpress.XtraEditors.Controls.ButtonPressedEventArgs e)
        {
            this.button_edit_prject_path_.Text = SelectFilePath("设置工程文件路径", FileType.PRO);
            string str_temp_vaild_area_path = this.button_edit_prject_path_.Text;
            str_temp_vaild_area_path = str_temp_vaild_area_path.Substring(0, str_temp_vaild_area_path.LastIndexOf("\\")) + "\\temp\\valid_area";
            flod_temp_vaild_files = new DirectoryInfo(str_temp_vaild_area_path);
            LoadXmlData(this.button_edit_prject_path_.Text);
        }
        //@设置正射影像路径 
        private void SelectDomPath(object sender, DevExpress.XtraEditors.Controls.ButtonPressedEventArgs e)
        {
            //this.button_edit_dom_path_.Text = SelectPath("选择DOM文件夹路径", FileType.DOM);
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.Description = "选择DOM文件夹路径";
            if (dialog.ShowDialog() == DialogResult.OK) this.button_edit_dom_path_.Text = dialog.SelectedPath;
            else
            {
                XtraMessageBox.Show("DOM文件夹路径获取失败!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            GetFilesFromFold(new DirectoryInfo(this.button_edit_dom_path_.Text), "DOM");
        }
        //@设置参考影像路径         
        private void SelectRefPath(object sender, DevExpress.XtraEditors.Controls.ButtonPressedEventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.Description = "选择REF文件夹路径";
            if (dialog.ShowDialog() == DialogResult.OK) this.button_edit_ref_path_.Text = dialog.SelectedPath;
            else
            {
                XtraMessageBox.Show("REF文件夹路径获取失败!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            GetFilesFromFold(new DirectoryInfo(this.button_edit_ref_path_.Text), "REF");
        }
        //@删除选中影像        
        private void DeleteSelectedImages(object sender, EventArgs e)
        {
            this.gridView1.DeleteSelectedRows();
            this.grid_control_.RefreshDataSource();
            CheckImagesNumber();
        }
        //@退出程序        
        private void CancleClicked(object sender, EventArgs e)
        {
            process_get_tasks.Kill();
            process_get_tasks.Close();
            this.Close();
        }
        //@执行质检任务        
        private void RunClicked(object sender, EventArgs e)
        {
            if (!SaveXmlData(this.button_edit_prject_path_.Text))
            {
                XtraMessageBox.Show("工程文件更新失败!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            string str = System.Environment.CurrentDirectory + "\\TskProcess\\ProTsk.exe";
            process_get_tasks = CreateProcessGetTasks(str, this.button_edit_prject_path_.Text);
            process_get_tasks.Start();

            this.splitContainer2.Panel2Collapsed = false;

            progressBarControl_vaild_area.Properties.Minimum = 0;
            progressBarControl_vaild_area.Properties.Maximum = int.Parse(this.text_edit_dom_number_.Text) + int.Parse(this.text_edit_ref_number_.Text);
            progressBarControl_vaild_area.Properties.Step = 1;
            progressBarControl_vaild_area.Properties.ProgressViewStyle = DevExpress.XtraEditors.Controls.ProgressViewStyle.Solid;
            progressBarControl_vaild_area.Position = 0;
            timer_progress_run.Start();   //启动刷新进度条
        }
        private void timer_progress_run_Tick(object sender, EventArgs e)
        {
            progressBarControl_vaild_area.Position = flod_temp_vaild_files.GetFiles().Count();

            if (progressBarControl_vaild_area.Position >= progressBarControl_vaild_area.Properties.Maximum)
            {
                timer_progress_run.Stop();
            }
        }
    }
    //@影像信息记录
    class RowDataRecord
    {
        public RowDataRecord(int n_id,string str_path, string str_type)
        {
            n_id_ = n_id;
            str_path_ = str_path;
            str_type_ = str_type;
        }
        //!编号
        public int n_id_ { get; set; }
        //!路径
        public string str_path_ { get; set; }
        //!类型
        public string str_type_ { get; set; }
    }
}
