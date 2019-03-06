#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDrag>
#include <QFile>
#include <QTextStream>

#include <PhyElement.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->nodeWidget->hide();
    ui->elemWidget->hide();
    ui->matWidget->hide();
    ui->dof3Widget->hide();
    ui->femPanel->addWidget(&fem_view);

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionMaterial_Panel, SIGNAL(triggered()), this, SLOT(on_showMatEditButton_clicked()));
    connect(ui->actionCreate_Node, SIGNAL(triggered()), this, SLOT(on_nodeButton_clicked()));
    connect(ui->actionCreate_Element, SIGNAL(triggered()), this, SLOT(on_elemButton_clicked()));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::selectNode(Node *n)
{
    if(selected==n) return;

    clearSelection();

    ui->node_selection->setText("<b>Node " + QString::number(n->id) + "<\b>");
    ui->dof1free->setChecked(!n->prescribed_x);
    ui->dof1fixed->setChecked(n->prescribed_x);
    ui->dof2free->setChecked(!n->prescribed_y);
    ui->dof2fixed->setChecked(n->prescribed_y);
    ui->dof3free->setChecked(!n->prescribed_3);
    ui->dof3fixed->setChecked(n->prescribed_3);
    ui->dof1input->setText(QString::number(n->val_x));
    ui->dof2input->setText(QString::number(n->val_y));
    ui->dof3input->setText(QString::number(n->val_3));
    ui->xInput->setText(QString::number(n->x));
    ui->yInput->setText(QString::number(n->y));

    ui->nodeWidget->show();
    ui->elemWidget->hide();

    selected = n;
    selected_is_node = true;
    n->select();
}

void MainWindow::updateNodeCoordinates(){
    Node* n = dynamic_cast<Node*>(selected);

    if(elem_type == etBar || elem_type == etBeam){
        n->y = 0;
        n->setPos(n->x*n->scale_factor, 0);
    }

    ui->xInput->setText(QString::number(n->x));
    ui->yInput->setText(QString::number(n->y));

    fem_view.scene->update();
}

void MainWindow::solve(){
    on_actionSave_Problem_triggered();

    std::string extensionless_name = current_filename.substr(0, current_filename.length()-4);
    qDebug() << extensionless_name.c_str();
    femSolver = new FEMSolver();
    femSolver->FEMSolve(extensionless_name, false);

    std::vector<PhyNode> nodes = femSolver->getNodes();

    VectorXd dof1(nodes.size());
    VectorXd dof2(nodes.size());

    for(int i = 0; i < nodes.size(); i++){
        PhyNode n = nodes[i];

        dof1(i) = n.dofs[0].value;
        if(dof >= 2) dof2(i) = n.dofs[1].value;
    }

    showDeformedGeometry(dof1, dof2);
}

void MainWindow::on_actionNew_Problem_triggered(){
    current_filename = "";
    setWindowTitle("QtFEM");

    if(solved) on_solveButton_clicked();

    fem_view.scene->clear();
    //for(Node* n : nodes) delete n;
    //for(Element* e : elements) delete e;
    nodes.clear();
    elements.clear();
    selected = nullptr;
    elem_mat_id.clear();
    elem_node_ids.clear();
    mat_params.clear();

    ui->nodeWidget->hide();
    ui->elemWidget->hide();
    ui->matWidget->hide();

    set2dim();
    set2dof();
    ui->trussButton->setChecked(true);

    ui->matEditSelect->setEnabled(false);
    ui->eInput->setEnabled(false);
    ui->aInput->setEnabled(false);
    ui->iInput->setEnabled(false);
}

void MainWindow::on_actionLoad_Problem_triggered(){
    QString file_path = QFileDialog::getOpenFileName(this,
        tr("Open Problem File"), "./", tr("Text Files (*.txt)"));

    if(file_path.isEmpty()){
        qDebug() << "Load cancelled";
        return;
    }

    on_actionNew_Problem_triggered();

    qDebug() << "Opened file: " << file_path;

    QRegExp folder_delimiters("\\\\|\\/");
    QString project_name = file_path.split(folder_delimiters).last();

    current_filename = file_path.toStdString();
    setWindowTitle("QtFEM (" + project_name + ")");

    fstream in(file_path.toStdString().c_str(), ios::in);
    femSolver = new FEMSolver();
    femSolver->Input(in);

    std::tuple<vector<PhyNode>, vector<PhyElement*>, map<int, VectorXd> > data = femSolver->getData();
    vector<PhyNode> phy_nodes = std::get<0>(data);
    vector<PhyElement*> phy_elems = std::get<1>(data);
    map<int, VectorXd> mats = std::get<2>(data);

    if(phy_nodes.size() > 0){
        dim = phy_nodes[0].coordinate.size();
        dof = phy_nodes[0].dofs.size();
    }else{
        QMessageBox msgBox;
        msgBox.setWindowTitle("Loading Error");
        msgBox.setText("Loaded file does not contain any nodes.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    elem_type = etTruss;
    if(phy_elems.size() > 0){
        elem_type = phy_elems[0]->eType;
    }

    int i = 1;
    for(PhyNode pn : std::get<0>(data)){
        Node* n = new Node(this, i++);
        nodes.push_back(n);
        fem_view.addItem(n);

        n->x = pn.coordinate(0);
        if(pn.coordinate.size()>=2){
            n->y = pn.coordinate(1);
            dim = 2;
        }
        n->setPos(n->x*n->scale_factor, -n->y*n->scale_factor);

        n->prescribed_x = pn.dofs[0].is_prescribed;
        n->val_x = n->prescribed_x ? pn.dofs[0].value : pn.dofs[0].force;

        if(dof >= 2){
            n->prescribed_y = pn.dofs[1].is_prescribed;
            n->val_y = n->prescribed_y ? pn.dofs[1].value : pn.dofs[1].force;
        }
        if(dof == 3){
            n->prescribed_3 = pn.dofs[2].is_prescribed;
            n->val_3 = n->prescribed_3 ? pn.dofs[2].value : pn.dofs[2].force;
        }
    }

    i = 0;
    for(std::pair<int,VectorXd> p : mats){
        elem_mat_id.push_back(i+1);

        if(p.first != i++){
            QMessageBox msgBox;
            msgBox.setWindowTitle("Loading Error");
            msgBox.setText("Mat ids are not ordered sequentially.");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            on_actionNew_Problem_triggered();
            return;
        }

        VectorXd m = p.second;

        switch(elem_type){
            case etBeam:
                mat_params.push_back(Vector3d(m(0),0,m(1)));
                break;
            case etFrame:
                mat_params.push_back(Vector3d(m(0),m(1),m(2)));
                break;
            default:
                mat_params.push_back(Vector3d(m(0),m(1),0));
        }
    }
    if(mat_params.size() > 0){
        ui->matEditSelect->setEnabled(true);
        ui->matEditSelect->setMinimum(1);
        ui->matEditSelect->setMaximum(mat_params.size());
        ui->matEditSelect->setValue(mat_params.size());

        ui->eInput->setEnabled(true);
        ui->aInput->setEnabled(true);
        ui->iInput->setEnabled(true);

        ui->matSelect->setMinimum(1);
        ui->matSelect->setMaximum(mat_params.size());
        ui->matSelect->setEnabled(true);
    }

    for(PhyElement* pe : phy_elems){
        int node_left = pe->eNodePtrs[0]->id;
        int node_right = pe->eNodePtrs[1]->id;
        int mat = pe->matID;

        elem_mat_id.push_back(mat);
        elem_node_ids.push_back(Vector2i(node_left, node_right));

        ui->element_selection->setText("<b>Element " + QString::number(elem_mat_id.size()+1) + " Creation<\b>");

        Element* e = new Element(*nodes[node_left-1], *nodes[node_right-1]);
        elements.push_back(e);
        fem_view.addItem(e);
    }

    fem_view.scene->update();
}

void MainWindow::on_actionSave_Problem_triggered(){
    if(current_filename == ""){
        on_actionSave_As_triggered();
        return;
    }

    QFile file(QString::fromStdString(current_filename));
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        stream << "dim " << dim << '\n';
        stream << "ndofpn " << dof << '\n';
        stream << "Nodes\n";
        stream << "nNodes " << nodes.size() << '\n';
        stream << "id crd\n";
        int i = 1;
        std::vector<int> prescribed_id;
        std::vector<int> prescribed_dof;
        std::vector<double> prescribed_value;
        std::vector<int> nonzero_id;
        std::vector<int> nonzero_dof;
        std::vector<double> nonzero_value;
        for(Node* n : nodes){
            stream << i << ' ' << n->x;
            if(dim==2) stream << ' ' << n->y;
            stream << '\n';

            if(n->prescribed_x){
                prescribed_id.push_back(i);
                prescribed_dof.push_back(1);
                prescribed_value.push_back(n->val_x);
            }
            if(dof >= 2 && n->prescribed_y){
                prescribed_id.push_back(i);
                prescribed_dof.push_back(2);
                prescribed_value.push_back(n->val_y);
            }
            if(dof == 3 && n->prescribed_3){
                prescribed_id.push_back(i);
                prescribed_dof.push_back(3);
                prescribed_value.push_back(n->val_3);
            }

            if(!n->prescribed_x && (n->val_x > 1e-13 || n->val_x < -1e-13)){
                nonzero_id.push_back(i);
                nonzero_dof.push_back(1);
                nonzero_value.push_back(n->val_x);
            }
            if(dof >= 2 && !n->prescribed_y && (n->val_y > 1e-13 || n->val_y < -1e-13)){
                nonzero_id.push_back(i);
                nonzero_dof.push_back(2);
                nonzero_value.push_back(n->val_y);
            }
            if(dof == 3 && !n->prescribed_3 && (n->val_3 > 1e-13 || n->val_3 < -1e-13)){
                nonzero_id.push_back(i);
                nonzero_dof.push_back(2);
                nonzero_value.push_back(n->val_3);
            }

            i++;
        }
        stream << "Elements\n";
        stream << "ne " << elements.size() << '\n';
        stream << "id elementType matID neNodes eNodes\n";
        for(int i = 1; i <= elements.size(); i++){
            stream << i << ' ' << static_cast<int>(elem_type) << ' '
                   << elem_mat_id[i-1] << " 2 "
                   << elem_node_ids[i-1](0) << ' ' << elem_node_ids[i-1](1) << '\n';
        }
        stream << "PrescribedDOF\n";
        stream << "np " << prescribed_id.size() << '\n';
        stream << "node node_dof_index value\n";
        for(int i = 0; i < prescribed_id.size(); i++){
            stream << prescribed_id[i] << ' ' << prescribed_dof[i] << ' ' << prescribed_value[i] << '\n';
        }
        stream << "FreeDofs\n";
        stream << "nNonZeroForceFDOFs " << nonzero_id.size() << '\n';
        stream << "node node_dof_index value\n";
        for(int i = 0; i < nonzero_id.size(); i++){
            stream << nonzero_id[i] << ' ' << nonzero_dof[i] << ' ' << nonzero_value[i] << '\n';
        }
        stream << "Materials\n";
        stream << "nMat " << mat_params.size() << '\n';
        stream << "id numPara Paras\n";

        i = 1;
        for(Vector3d m : mat_params){
            if(elem_type==etBeam){
                stream << i++ << " 2 " << m(0) << ' ' << m(2) << '\n';
            }else if(elem_type==etFrame){
                stream << i++ << " 3 " << m(0) << ' ' << m(1) << ' ' << m(2) << '\n';
            }else{
                stream << i++ << " 2 " << m(0) << ' ' << m(1) << '\n';
            }
        }
    }

    file.close();
}

void MainWindow::on_actionSave_As_triggered(){
    QString file_path = QFileDialog::getSaveFileName(this,
        tr("Save Problem File"), "./", tr("Text Files (*.txt)"));

    if(file_path.isEmpty()){
        qDebug() << "Save cancelled";
        return;
    }

    QRegExp folder_delimiters("\\\\|\\/");
    QString project_name = file_path.split(folder_delimiters).last();

    qDebug() << "Saved file: " << file_path;

    current_filename = file_path.toStdString();
    setWindowTitle("QtFEM (" + project_name + ")");

    on_actionSave_Problem_triggered();
}

void MainWindow::on_showMatEditButton_clicked(){
    if(ui->matWidget->isHidden()){
        ui->matWidget->show();
        ui->actionMaterial_Panel->setChecked(true);
    }else{
        ui->matWidget->hide();
        ui->actionMaterial_Panel->setChecked(false);
    }
}

void MainWindow::on_nodeButton_clicked(){
    Node* n = new Node(this, nodes.size()+1);
    nodes.push_back(n);
    fem_view.addItem(n);
    selectNode(n);

    qDebug() << "New node added";
}

void MainWindow::on_elemButton_clicked(){
    clearSelection();

    ui->matSelect->setMinimum(1);
    ui->matSelect->setMaximum(mat_params.size());

    ui->node1select->setMinimum(1);
    ui->node2select->setMinimum(1);
    ui->node1select->setMaximum(nodes.size());
    ui->node2select->setMaximum(nodes.size());

    bool has_mats = (mat_params.size() > 0);
    bool has_nodes = (nodes.size() > 1);
    bool ready = has_mats && has_nodes;

    ui->matSelect->setEnabled(has_mats);
    ui->node1select->setEnabled(has_nodes);
    ui->node2select->setEnabled(has_nodes);
    ui->elemCreate->setEnabled(ready);

    ui->element_selection->setText("<b>Element " + QString::number(elem_mat_id.size()+1) + " Creation<\b>");
    ui->nodeWidget->hide();
    ui->elemWidget->show();

    selected_is_node = false;
}

void MainWindow::clearSelection(){
    if(selected==nullptr) return;
    if(selected_is_node) static_cast<Node*>(selected)->deselect();
    qDebug() << "selection cleared.";
}

void MainWindow::set1dim(){
    ui->yInput->setEnabled(false);
    ui->yInput->setText("0");
    dim = 1;

    for(Node* n : nodes){
        n->y = 0;
        n->setPos(n->x*n->scale_factor, 0);
        n->update();
    }

    fem_view.scene->update();
}

void MainWindow::set2dim(){
    ui->yInput->setEnabled(true);
    dim = 2;
}

void MainWindow::set1dof(){
    ui->dof2widget->hide();
    ui->dof3Widget->hide();
    dof = 1;
}

void MainWindow::set2dof(){
    ui->dof2widget->show();
    ui->dof3Widget->hide();
    dof = 2;
}

void MainWindow::set3dof(){
    ui->dof2widget->show();
    ui->dof3Widget->show();
    dof = 3;
}

void MainWindow::showDeformedGeometry(VectorXd dof1, VectorXd dof2){
    int i = 0;

    switch(elem_type){
    case etTruss:
        for(Node* n : nodes){
            n->setPos((n->x+dof1(i))*n->scale_factor,
                      -(n->y+dof2(i))*n->scale_factor);
            i++;
        }
        break;
    case etFrame:
        for(Node* n : nodes){
            n->setPos((n->x+dof1(i))*n->scale_factor,
                      -(n->y+dof2(i))*n->scale_factor);
            i++;
        }
        break;
    case etBar:
        for(Node* n : nodes){
            n->setPos((n->x+dof1(i))*n->scale_factor,
                      0);
            i++;
        }
        break;
    case etBeam:
        for(Node* n : nodes){
            n->setPos(n->x*n->scale_factor,
                      -dof1(i)*n->scale_factor);
            i++;
        }
        break;
    }

    fem_view.scene->update();
}

void MainWindow::on_actionZoom_In_triggered(){
    fem_view.scaleView(pow((double)2, 1));
}

void MainWindow::on_actionZoom_Out_triggered(){
    fem_view.scaleView(pow((double)2, -1));
}

void MainWindow::on_actionReset_Zoom_triggered(){
    QTransform I;
    I.setMatrix(1,0,0,0,1,0,0,0,1);
    fem_view.setTransform(I);
}

void MainWindow::on_dof1free_clicked(){
    dynamic_cast<Node*>(selected)->prescribed_x = false;
    ui->dof1valueLabel->setText("Force:");
}

void MainWindow::on_dof1fixed_clicked(){
    dynamic_cast<Node*>(selected)->prescribed_x = true;
    ui->dof1valueLabel->setText("ux:");
}

void MainWindow::on_dof2free_clicked(){
    dynamic_cast<Node*>(selected)->prescribed_y = false;
    ui->dof2valueLabel->setText("Force:");
}

void MainWindow::on_dof2fixed_clicked(){
    dynamic_cast<Node*>(selected)->prescribed_y = true;
    ui->dof2valueLabel->setText("uy:");
}

void MainWindow::on_dof3free_clicked(){
    dynamic_cast<Node*>(selected)->prescribed_3 = false;
    ui->dof3valueLabel->setText("Moment:");
}

void MainWindow::on_dof3fixed_clicked(){
    dynamic_cast<Node*>(selected)->prescribed_3 = true;
    ui->dof3valueLabel->setText("θz:");
}

void MainWindow::on_dof1input_editingFinished(){
    double val = ui->dof1input->text().toDouble();
    dynamic_cast<Node*>(selected)->val_x = val;
}

void MainWindow::on_dof2input_editingFinished(){
    double val = ui->dof2input->text().toDouble();
    dynamic_cast<Node*>(selected)->val_y = val;
}

void MainWindow::on_dof3input_editingFinished(){
    double val = ui->dof3input->text().toDouble();
    dynamic_cast<Node*>(selected)->val_3 = val;
}

void MainWindow::on_xInput_editingFinished(){
    double x = ui->xInput->text().toDouble();
    Node* n = dynamic_cast<Node*>(selected);
    n->x = x;
    n->setPos(x*n->scale_factor, -n->y*n->scale_factor);
}

void MainWindow::on_yInput_editingFinished(){
    double y = ui->yInput->text().toDouble();
    Node* n = dynamic_cast<Node*>(selected);
    n->y = y;
    n->setPos(n->x*n->scale_factor, -y*n->scale_factor);
}

void MainWindow::on_trussButton_clicked(){
    elem_type = ElementType::etTruss;
    set2dim();
    set2dof();

    qDebug() << "switched to truss type";
}

void MainWindow::on_frameButton_clicked(){
    elem_type = ElementType::etFrame;
    set2dim();
    set3dof();

    qDebug() << "switched to frame type";
}

void MainWindow::on_barButton_clicked(){
    elem_type = ElementType::etBar;
    set1dim();
    set1dof();

    qDebug() << "switched to bar type";
}

void MainWindow::on_beamButton_clicked(){
    elem_type = ElementType::etBeam;
    set1dim();
    set2dof();

    qDebug() << "switched to beam type";
}

void MainWindow::on_newMaterial_clicked(){
    mat_params.push_back(Vector3d::Zero());
    ui->matEditSelect->setEnabled(true);
    ui->matEditSelect->setMinimum(1);
    ui->matEditSelect->setMaximum(mat_params.size());
    ui->matEditSelect->setValue(mat_params.size());

    ui->eInput->setEnabled(true);
    ui->eInput->setText("0");

    ui->aInput->setEnabled(true);
    ui->aInput->setText("0");

    ui->iInput->setEnabled(true);
    ui->iInput->setText("0");

    ui->matSelect->setMinimum(1);
    ui->matSelect->setMaximum(mat_params.size());
    ui->matSelect->setEnabled(true);
    if(nodes.size() > 1) ui->elemCreate->setEnabled(true);
}

void MainWindow::on_matEditSelect_valueChanged(int arg1){
    ui->eInput->setText(QString::number(mat_params[arg1-1](0)));
    ui->aInput->setText(QString::number(mat_params[arg1-1](1)));
    ui->iInput->setText(QString::number(mat_params[arg1-1](2)));
}

void MainWindow::on_eInput_editingFinished(){
    int mat = ui->matEditSelect->value();
    double val = ui->eInput->text().toDouble();
    mat_params[mat-1](0) = val;
}

void MainWindow::on_aInput_editingFinished(){
    int mat = ui->matEditSelect->value();
    double val = ui->aInput->text().toDouble();
    mat_params[mat-1](1) = val;
}

void MainWindow::on_iInput_editingFinished(){
    int mat = ui->matEditSelect->value();
    double val = ui->iInput->text().toDouble();
    mat_params[mat-1](2) = val;
}

void MainWindow::on_elemCreate_clicked(){
    int node_left = ui->node1select->value();
    int node_right = ui->node2select->value();
    int mat = ui->matSelect->value();

    if(node_left == node_right){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Editor Error");
        msgBox.setText("Node 1 and Node 2 cannot be the same.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    elem_mat_id.push_back(mat);
    elem_node_ids.push_back(Vector2i(node_left, node_right));

    ui->element_selection->setText("<b>Element " + QString::number(elem_mat_id.size()+1) + " Creation<\b>");

    Element* e = new Element(*nodes[node_left-1], *nodes[node_right-1]);
    elements.push_back(e);
    fem_view.addItem(e);
    fem_view.scene->update();
}

void MainWindow::on_solveButton_clicked(){
    if(!solved){
        if(elements.size()==0){
            QMessageBox msgBox;
            msgBox.setWindowTitle("Solver Error");
            msgBox.setText("Cannot solve system without any elements.");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            return;
        }

        solve();

        solved = true;

        //Disable changes except button
        ui->editorWidget->setEnabled(false);
        ui->nodeWidget->hide();
        ui->elemWidget->hide();
        ui->matWidget->hide();
        ui->nodeWidget->setEnabled(false);
        ui->actionMaterial_Panel->setEnabled(false);
        ui->actionCreate_Node->setEnabled(false);
        ui->actionCreate_Element->setEnabled(false);
        for(Node* n : nodes) n->setFlag(QGraphicsItem::ItemIsMovable, false);

        ui->solveButton->setText("Edit");
    }else{
        //Visualize undeformed
        for(Node* n : nodes)
            n->setPos(n->x*n->scale_factor, -n->y*n->scale_factor);

        //Enable changes
        ui->nodeWidget->setEnabled(true);
        ui->editorWidget->setEnabled(true);
        ui->actionMaterial_Panel->setEnabled(true);
        ui->actionCreate_Node->setEnabled(true);
        ui->actionCreate_Element->setEnabled(true);
        for(Node* n : nodes) n->setFlag(QGraphicsItem::ItemIsMovable);

        solved = false;

        ui->solveButton->setText("SOLVE");

        fem_view.scene->update();
    }
}
