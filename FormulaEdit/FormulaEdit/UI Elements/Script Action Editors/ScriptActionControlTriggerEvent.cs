﻿using System.Windows.Forms;

namespace FormulaEdit.UI_Elements.Script_Action_Editors
{
    public partial class ScriptActionControlTriggerEvent : UserControl
    {
        internal ScriptActionControlTriggerEvent(MudData.FormulaActionTriggerEvent action)
        {
            InitializeComponent();

            TargetComboBox.Text = action.target;
            EventComboBox.Text = action.@event;
            DelayTextBox.Text = action.delay;

            if (action.@params != null)
            {
                foreach (var kvp in action.@params)
                {
                    ParamDataGridView.Rows.Add(new object[] { kvp.Key, kvp.Value });
                }
            }
        }

        internal MudData.FormulaActionTriggerEvent GenerateMudDataAction()
        {
            var ret = new MudData.FormulaActionTriggerEvent();
            ret.action = "TriggerEvent";
            ret.@event = EventComboBox.Text;
            ret.target = TargetComboBox.Text;
            ret.delay = DelayTextBox.Text;

            foreach (DataGridViewRow row in ParamDataGridView.Rows)
            {
                if (row.Cells.Count != 2)
                    continue;

                if (row.Cells[0].Value == null)
                    continue;

                if (row.Cells[1].Value == null)
                    continue;

                string key = row.Cells[0].Value.ToString();
                string value = row.Cells[1].Value.ToString();

                ret.@params[key] = value;
            }

            return ret;
        }
    }
}
