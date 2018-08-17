#ifndef _TIMEMANAGER_H
#define _TIMEMANAGER_H

#include <Core/Math/LinearAlgebra.hpp>

#include "Config.h"

namespace PBD
{
	class TimeManager
	{
	private:
        Scalar time;
		static TimeManager *current;
        Scalar h;

	public:
		TimeManager ();
		~TimeManager ();

		// Singleton
		static TimeManager* getCurrent ();
		static void setCurrent (TimeManager* tm);
		static bool hasCurrent();

        Scalar getTime();
        void setTime(Scalar t);
        Scalar getTimeStepSize();
        void setTimeStepSize(Scalar tss);
	};
}

#endif
