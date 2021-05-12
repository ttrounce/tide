#ifndef TIDE_ACTIVITY_HOLDER_H
#define TIDE_ACTIVITY_HOLDER_H

#include "component.h"

struct ActivityHolder
{
public:
    void SetActiveComponent(Component* component);
    Component* GetActiveComponent();
    Component* GetPreviousComponent();
private:
    Component* activeComponent;
    Component* previousComponent;
};

#endif // TIDE_ACTIVITY_HOLDER_H