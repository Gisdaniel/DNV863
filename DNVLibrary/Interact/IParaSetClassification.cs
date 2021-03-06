﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DNVLibrary.Interact
{
    class IParaSetClassification : AppBase
    {
        public IParaSetClassification(UCDNV863 Root)
            : base(Root)
        {
        }

        LoadInput.LoadClassification uc;
        internal override void load()
        {
            if (uc == null)
                uc = new LoadInput.LoadClassification() { Margin = new System.Windows.Thickness(20, 100, 20, 40) };
            root.grdContent.Children.Add(uc);
         
        }

        internal override void unload()
        {
            if (panel != null) (panel as BaseIPanel).unload();

            if (uc != null && root.grdContent.Children.Contains(uc))
                root.grdContent.Children.Remove(uc);
            uc = null;
        }

        internal void show()
        {
            if (uc != null && root.grdContent.Children.Contains(uc))
                uc.Visibility = System.Windows.Visibility.Visible;

            MyBaseControls.StatusBarTool.StatusBarTool.tipsInfo.curDomain = "互动_参数设置_分类负荷特性";

        }
        internal void hide()
        {
            if (uc != null && root.grdContent.Children.Contains(uc))
                uc.Visibility= System.Windows.Visibility.Collapsed;

            MyBaseControls.StatusBarTool.StatusBarTool.tipsInfo.curDomain = "互动_参数设置";
        }
    }
}
