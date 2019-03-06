#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <CFEMTypes_Global.h>
#include <FEMSolver.h>
#include <femview.h>
#include <node.h>
#include <element.h>

//Need nodes and elements with click events
//Use elasticnodes example as basis - it has everything I need

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    bool solved = false;
    FemView fem_view;
    FEMSolver* femSolver;
    std::string current_filename = "";

    QGraphicsItem* selected = nullptr;
    bool selected_is_node;

    std::vector<Node*> nodes;
    std::vector<Element*> elements;

    //Data members directly mirror the input file format
    ElementType elem_type = etTruss; //The element types are totally homogenous

    std::vector<int> elem_mat_id;
    std::vector<Vector2i> elem_node_ids; //Every element in our simple implementation has two nodes
    std::vector<Vector3d> mat_params;

    int dof = 2;
    int dim = 2;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void selectNode(Node* n);
    void updateNodeCoordinates();
    void solve();

private slots:
    void on_actionNew_Problem_triggered();
    void on_actionLoad_Problem_triggered();
    void on_actionSave_Problem_triggered();
    void on_actionSave_As_triggered();
    void on_showMatEditButton_clicked();
    void on_nodeButton_clicked();
    void on_elemButton_clicked();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionReset_Zoom_triggered();
    void on_dof1free_clicked();
    void on_dof1fixed_clicked();
    void on_dof2free_clicked();
    void on_dof2fixed_clicked();
    void on_dof3free_clicked();
    void on_dof3fixed_clicked();
    void on_dof1input_editingFinished();
    void on_dof2input_editingFinished();
    void on_dof3input_editingFinished();
    void on_xInput_editingFinished();
    void on_yInput_editingFinished();
    void on_trussButton_clicked();
    void on_frameButton_clicked();
    void on_barButton_clicked();
    void on_beamButton_clicked();
    void on_newMaterial_clicked();
    void on_matEditSelect_valueChanged(int arg1);
    void on_eInput_editingFinished();
    void on_aInput_editingFinished();
    void on_iInput_editingFinished();
    void on_elemCreate_clicked();
    void on_solveButton_clicked();

private:
    void clearSelection();
    void set1dim();
    void set2dim();
    void set1dof();
    void set2dof();
    void set3dof();
    void showDeformedGeometry(VectorXd dof1, VectorXd dof2);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
