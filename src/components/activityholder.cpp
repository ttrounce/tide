#include "activityholder.h"

void ActivityHolder::SetActiveComponent(Component* component)
{
    this->previousComponent = activeComponent;
    this->activeComponent = component;
}

Component* ActivityHolder::GetActiveComponent()
{
    return this->activeComponent;
}
Component* ActivityHolder::GetPreviousComponent()
{
    return this->previousComponent;
}