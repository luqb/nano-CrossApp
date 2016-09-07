

#include "CANotificationCenter.h"



NS_CC_BEGIN

static CANotificationCenter *s_sharedNotifCenter = NULL;

CANotificationCenter::CANotificationCenter()
: m_scriptHandler(0)
{

}

CANotificationCenter::~CANotificationCenter()
{
    m_observers.clear();
}

CANotificationCenter *CANotificationCenter::getInstance(void)
{
    if (!s_sharedNotifCenter)
    {
        s_sharedNotifCenter = new (std::nothrow) CANotificationCenter();
    }
    return s_sharedNotifCenter;
}

void CANotificationCenter::destroyInstance(void)
{
    CC_SAFE_RELEASE_NULL(s_sharedNotifCenter);
}

//
// internal functions
//
bool CANotificationCenter::observerExisted(CAObject *target, const std::string& name)
{
    for (auto & observer : m_observers)
    {
        if (!observer)
            continue;
        
        if (observer->getName() == name && observer->getTarget() == target)
            return true;
    }
    return false;
}

//
// observer functions
//
void CANotificationCenter::addObserver(CAObject *target, SEL_CallFuncO selector, const std::string& name, CAObject *obj)
{
    if (this->observerExisted(target, name))
        return;
    
    CANotificationObserver *observer = new CANotificationObserver(target, selector, name, obj);
    if (!observer)
        return;
    
    observer->autorelease();
    m_observers.pushBack(observer);
}

void CANotificationCenter::removeObserver(CAObject *target, const std::string& name)
{
    for (auto itr=m_observers.begin(); itr!=m_observers.end();)
    {
        CANotificationObserver* observer = *itr;
        if (!observer)
            continue;
        
        if (observer->getName() == name && observer->getTarget() == target)
        {
            itr = m_observers.erase(itr);
			break;
        }
        else
        {
            ++itr;
        }
    }
}

int CANotificationCenter::removeAllObservers(CAObject *target)
{
	int size = 0;
    for (auto itr=m_observers.begin(); itr!=m_observers.end();)
    {
        CANotificationObserver* observer = *itr;
        if (!observer)
            continue;

        if (observer->getTarget() == target)
        {
			itr = m_observers.erase(itr);
			++size;
        }
		else
		{
			++itr;
		}
    }

	return size;
}

void CANotificationCenter::registerScriptObserver( CAObject *target, int handler, const std::string& name)
{
    
    if (this->observerExisted(target, name))
        return;
    
    CANotificationObserver *observer = new CANotificationObserver(target, NULL, name, NULL);
    if (!observer)
        return;
    
    observer->setHandler(handler);
    observer->autorelease();
    m_observers.pushBack(observer);
}

void CANotificationCenter::unregisterScriptObserver(CAObject *target, const std::string& name)
{        
    for (auto itr=m_observers.begin(); itr!=m_observers.end();)
    {
        CANotificationObserver* observer = *itr;
        if (!observer)
            continue;
            
        if (observer->getName() == name && observer->getTarget() == target)
        {
            itr = m_observers.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

void CANotificationCenter::postNotification(const std::string& name, CAObject *object)
{
    auto ObserversCopy = CADeque<CANotificationObserver*>(m_observers);
    
    for (auto itr=ObserversCopy.begin(); itr!=ObserversCopy.end(); ++itr)
    {
        CANotificationObserver* observer = *itr;
        if (!observer)
            continue;
        
        if (observer->getName() == name && (observer->getObject() == object || observer->getObject() == NULL || object == NULL))
        {
            if (0 == observer->getHandler())
            {
                observer->performSelector(object);
            }
        }
    }
}

void CANotificationCenter::postNotification(const std::string& name)
{
    this->postNotification(name, NULL);
}

int CANotificationCenter::getObserverHandlerByName(const std::string& name)
{
    if (name.empty())
    {
        return -1;
    }
    
    for (auto itr=m_observers.begin(); itr!=m_observers.end(); ++itr)
    {
        CANotificationObserver* observer = *itr;
        if (NULL == observer)
            continue;
        
        if (observer->getName() == name)
        {
            return observer->getHandler();
            break; 
        }
    }
    
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
///
/// CANotificationObserver
///
////////////////////////////////////////////////////////////////////////////////
CANotificationObserver::CANotificationObserver(CAObject *target, SEL_CallFuncO selector, const std::string& name, CAObject *obj)
{
    m_target = target;
    m_selector = selector;
    m_object = obj;
    m_name = name;
    m_nHandler = 0;
}

CANotificationObserver::~CANotificationObserver()
{

}

void CANotificationObserver::performSelector(CAObject *obj)
{
    if (m_target)
    {
		if (obj)
        {
			(m_target->*m_selector)(obj);
		}
        else
        {
			(m_target->*m_selector)(m_object);
		}
    }
}

CAObject *CANotificationObserver::getTarget()
{
    return m_target;
}

SEL_CallFuncO CANotificationObserver::getSelector()
{
    return m_selector;
}

const std::string& CANotificationObserver::getName()
{
    return m_name;
}

CAObject *CANotificationObserver::getObject()
{
    return m_object;
}

int CANotificationObserver::getHandler()
{
    return m_nHandler;
}

void CANotificationObserver::setHandler(int var)
{
    m_nHandler = var;
}

NS_CC_END
