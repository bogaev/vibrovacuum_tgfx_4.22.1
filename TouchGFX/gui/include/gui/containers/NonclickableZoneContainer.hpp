#ifndef NONCLICKABLEZONECONTAINER_HPP
#define NONCLICKABLEZONECONTAINER_HPP

#include <gui_generated/containers/NonclickableZoneContainerBase.hpp>

class NonclickableZoneContainer : public NonclickableZoneContainerBase
{
public:
    NonclickableZoneContainer();
    virtual ~NonclickableZoneContainer() {}

    virtual void initialize();
protected:
};

#endif // NONCLICKABLEZONECONTAINER_HPP
