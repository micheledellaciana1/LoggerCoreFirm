#ifndef VMODE_H
#define VMODE_H

/**
 * @brief This class is a pure virtual class. It rappresent the basic routine that is handled from LoopManager instance.
 * @details When you load a pointer of VMode to LoopManger, LoopManager will call loop() function untill the mode is removed or i disabled. It actually simulate in a sequential mode the behaviur of a thread.
 */
class VMode
{
protected:
    bool _isActive;
    String _name;

protected:
    VMode()
    {
        _isActive = true;
    }

public:
    virtual ~VMode() {}

    VMode(String name)
    {
        _name = name;
        _isActive = true;
    }

    String getName()
    {
        return _name;
    }

    void setName(String name)
    {
        _name = name;
    }

    void Enable()
    {
        _isActive = true;
    }

    void Disable()
    {
        _isActive = false;
    }

    bool isActive()
    {
        return _isActive;
    }

    virtual void loop() = 0;
};

#endif // VMODE_H