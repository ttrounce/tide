#ifndef TIDE_COMPONENT_H
#define TIDE_COMPONENT_H

class Component
{
protected:
    bool rendered;
    bool active;

public:
    virtual void Batch() = 0;
    virtual void TakeInput(int inputType, unsigned int codepoint) = 0;
    virtual ~Component() {};
};

class ActableComponent : public Component
{
public:
    void SetActive(bool active);
    bool IsActive();

    void SetRendered(bool rendered);
    bool IsRendered();
};

#endif // TIDE_COMPONENT_H