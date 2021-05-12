#include "component.h"

bool ActableComponent::IsActive()
{
    return active;
}

void ActableComponent::SetActive(bool active)
{
    this->active = active;
}

void ActableComponent::SetRendered(bool rendered)
{
    this->rendered = rendered;
}

bool ActableComponent::IsRendered()
{
    return rendered;
}

// template <typename SpecialInputType>
// Component<SpecialInputType>::~Component() {}