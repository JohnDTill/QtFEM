#ifndef PHY_DOF__H
#define PHY_DOF__H

struct PhyDof{
public:
    bool is_prescribed = false;
    int pos = -1; // so the code definitely crashes if it goes unassigned
    double value = 0.0;
    double force = 0.0;
};

#endif
