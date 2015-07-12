#ifndef RADIUMENGINE_SINGLETON_HPP
#define RADIUMENGINE_SINGLETON_HPP

#include <cassert>
#include <cstdio>
#include <typeinfo>

namespace Ra
{

template <typename T>
class Singleton
{
public:
	template <typename... Args>
	static T* createInstance(const Args&... args)
	{
		if (nullptr == s_instance)
		{
			s_instance = new T(args...);
		}
		else 
		{
			fprintf(stderr, "Singleton<%s> has already been instanciated.\n",
					typeid(T).name());
		}

		return s_instance;
	}

	static T* getInstancePtr()
	{
		if (!s_instance)
		{
			fprintf(stderr, "Singleton<%s>::getInstancePtr() called on an initialized instance.\n",
					typeid(T).name());
		}

		return s_instance;
	}

	static T& getInstanceRef()
	{
		assert(s_instance != nullptr);
		return *s_instance;
	}

	static void destroyInstance()
	{
		if (s_instance)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}

protected:
	Singleton() {}
	~Singleton() {}

private:
	Singleton(const Singleton<T>&) = delete;
	void operator=(const Singleton<T>&) = delete;

private:
	static T* s_instance;
};

template <typename T> T* Singleton<T>::s_instance = nullptr;

} // namespace Ra

#endif // RADIUMENGINE_SINGLETON_HPP