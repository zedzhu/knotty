/* $Id: ScopeGuard.h,v 1.5 2012/09/20 03:21:47 jiagui Exp $ */
#ifndef SCOPEGUARD_H_
#define SCOPEGUARD_H_

#include "xsdef.h"

/*
  Scopeguard, by Andrei Alexandrescu and Petru Marginean, December 2000.
  Modified by Joshua Lehrer, FactSet Research Systems, November 2005.
*/

template <class T>
class RefHolder
{
	T& ref_;
public:
	RefHolder(T& ref) : ref_(ref) {}
	operator T& () const 
	{
		return ref_;
	}
private:
	// Disable assignment - not implemented
	RefHolder& operator=(const RefHolder&);
};

template <class T>
inline RefHolder<T> ByRef(T& t)
{
	return RefHolder<T>(t);
}

class ScopeGuardImplBase
{
	ScopeGuardImplBase& operator =(const ScopeGuardImplBase&);
protected:
	~ScopeGuardImplBase()
	{
	}
	ScopeGuardImplBase(const ScopeGuardImplBase& other) noexcept 
		: dismissed_(other.dismissed_)
	{
		other.dismiss();
	}
	template <typename J>
	static void SafeExecute(J& j) noexcept 
	{
		if (!j.dismissed_)
			try
			{
				j.execute();
			}
			catch(...)
			{
			}
	}
	
	mutable bool dismissed_;
public:
	ScopeGuardImplBase() noexcept : dismissed_(false) 
	{
	}
	void dismiss() const noexcept 
	{
		dismissed_ = true;
	}
};

typedef const ScopeGuardImplBase& ScopeGuard;

template <typename F>
class ScopeGuardImpl0 : public ScopeGuardImplBase
{
public:
	static ScopeGuardImpl0<F> MakeGuard(F fun)
	{
		return ScopeGuardImpl0<F>(fun);
	}
	~ScopeGuardImpl0() noexcept 
	{
		SafeExecute(*this);
	}
	void execute() 
	{
		if (!dismissed_)
		{
			fun_();
			dismiss();
		}
	}
protected:
	ScopeGuardImpl0(F fun) : fun_(fun) 
	{
	}
	F fun_;
};

template <typename F> 
inline ScopeGuardImpl0<F> MakeGuard(F fun)
{
	return ScopeGuardImpl0<F>::MakeGuard(fun);
}

template <typename F, typename P1>
class ScopeGuardImpl1 : public ScopeGuardImplBase
{
public:
	static ScopeGuardImpl1<F, P1> MakeGuard(F fun, P1 p1)
	{
		return ScopeGuardImpl1<F, P1>(fun, p1);
	}
	~ScopeGuardImpl1() noexcept 
	{
		SafeExecute(*this);
	}
	void execute()
	{
		if (!dismissed_)
		{
			fun_(p1_);
			dismiss();
		}
	}
protected:
	ScopeGuardImpl1(F fun, P1 p1) : fun_(fun), p1_(p1) 
	{
	}
	F fun_;
	const P1 p1_;
};

template <typename F, typename P1> 
inline ScopeGuardImpl1<F, P1> MakeGuard(F fun, P1 p1)
{
	return ScopeGuardImpl1<F, P1>::MakeGuard(fun, p1);
}

template <typename F, typename P1, typename P2>
class ScopeGuardImpl2: public ScopeGuardImplBase
{
public:
	static ScopeGuardImpl2<F, P1, P2> MakeGuard(F fun, P1 p1, P2 p2)
	{
		return ScopeGuardImpl2<F, P1, P2>(fun, p1, p2);
	}
	~ScopeGuardImpl2() noexcept 
	{
		SafeExecute(*this);
	}
	void execute()
	{
		if (!dismissed_)
		{
			fun_(p1_, p2_);
			dismiss();
		}
	}
protected:
	ScopeGuardImpl2(F fun, P1 p1, P2 p2) : fun_(fun), p1_(p1), p2_(p2) 
	{
	}
	F fun_;
	const P1 p1_;
	const P2 p2_;
};

template <typename F, typename P1, typename P2>
inline ScopeGuardImpl2<F, P1, P2> MakeGuard(F fun, P1 p1, P2 p2)
{
	return ScopeGuardImpl2<F, P1, P2>::MakeGuard(fun, p1, p2);
}

template <typename F, typename P1, typename P2, typename P3>
class ScopeGuardImpl3 : public ScopeGuardImplBase
{
public:
	static ScopeGuardImpl3<F, P1, P2, P3> MakeGuard(F fun, P1 p1, P2 p2, P3 p3)
	{
		return ScopeGuardImpl3<F, P1, P2, P3>(fun, p1, p2, p3);
	}
	~ScopeGuardImpl3() noexcept 
	{
		SafeExecute(*this);
	}
	void execute()
	{
		if (!dismissed_)
		{
			fun_(p1_, p2_, p3_);
			dismiss();
		}
	}
protected:
	ScopeGuardImpl3(F fun, P1 p1, P2 p2, P3 p3) : fun_(fun), p1_(p1), p2_(p2), p3_(p3) 
	{
	}
	F fun_;
	const P1 p1_;
	const P2 p2_;
	const P3 p3_;
};

template <typename F, typename P1, typename P2, typename P3>
inline ScopeGuardImpl3<F, P1, P2, P3> MakeGuard(F fun, P1 p1, P2 p2, P3 p3)
{
	return ScopeGuardImpl3<F, P1, P2, P3>::MakeGuard(fun, p1, p2, p3);
}

//************************************************************

template <class Obj, typename MemFun>
class ObjScopeGuardImpl0 : public ScopeGuardImplBase
{
public:
	static ObjScopeGuardImpl0<Obj, MemFun> MakeObjGuard(Obj& obj, MemFun memFun)
	{
		return ObjScopeGuardImpl0<Obj, MemFun>(obj, memFun);
	}
	~ObjScopeGuardImpl0() noexcept 
	{
		SafeExecute(*this);
	}
	void execute() 
	{
		if (!dismissed_)
		{
			(obj_.*memFun_)();
			dismiss();
		}
	}
protected:
	ObjScopeGuardImpl0(Obj& obj, MemFun memFun) 
		: obj_(obj), memFun_(memFun) {}
	Obj& obj_;
	MemFun memFun_;
};

template <class Obj, typename MemFun>
inline ObjScopeGuardImpl0<Obj, MemFun> MakeObjGuard(Obj& obj, MemFun memFun)
{
	return ObjScopeGuardImpl0<Obj, MemFun>::MakeObjGuard(obj, memFun);
}

template <typename Ret, class Obj1, class Obj2>
inline ObjScopeGuardImpl0<Obj1, Ret(Obj2::*)()> MakeGuard(Ret(Obj2::*memFun)(), Obj1 &obj)
{
	return ObjScopeGuardImpl0<Obj1, Ret(Obj2::*)()>::MakeObjGuard(obj, memFun);
}


template <class Obj, typename MemFun, typename P1>
class ObjScopeGuardImpl1 : public ScopeGuardImplBase
{
public:
	static ObjScopeGuardImpl1<Obj, MemFun, P1> MakeObjGuard(Obj& obj, MemFun memFun, P1 p1)
	{
		return ObjScopeGuardImpl1<Obj, MemFun, P1>(obj, memFun, p1);
	}
	~ObjScopeGuardImpl1() noexcept 
	{
		SafeExecute(*this);
	}
	void execute() 
	{
		if (!dismissed_)
		{
			(obj_.*memFun_)(p1_);
			dismiss();
		}
	}
protected:
	ObjScopeGuardImpl1(Obj& obj, MemFun memFun, P1 p1) 
		: obj_(obj), memFun_(memFun), p1_(p1) {}
	Obj& obj_;
	MemFun memFun_;
	const P1 p1_;
};

template <class Obj, typename MemFun, typename P1>
inline ObjScopeGuardImpl1<Obj, MemFun, P1> MakeObjGuard(Obj& obj, MemFun memFun, P1 p1)
{
	return ObjScopeGuardImpl1<Obj, MemFun, P1>::MakeObjGuard(obj, memFun, p1);
}

template <typename Ret, class Obj1, class Obj2, typename P1a, typename P1b>
inline ObjScopeGuardImpl1<Obj1, Ret(Obj2::*)(P1a), P1b> MakeGuard(Ret(Obj2::*memFun)(P1a), Obj1 &obj, P1b p1)
{
	return ObjScopeGuardImpl1<Obj1, Ret(Obj2::*)(P1a), P1b>::MakeObjGuard(obj, memFun, p1);
}


template <class Obj, typename MemFun, typename P1, typename P2>
class ObjScopeGuardImpl2 : public ScopeGuardImplBase
{
public:
	static ObjScopeGuardImpl2<Obj, MemFun, P1, P2> MakeObjGuard(Obj& obj, MemFun memFun, P1 p1, P2 p2)
	{
		return ObjScopeGuardImpl2<Obj, MemFun, P1, P2>(obj, memFun, p1, p2);
	}
	~ObjScopeGuardImpl2() noexcept 
	{
		SafeExecute(*this);
	}
	void execute() 
	{
		if (!dismissed_)
		{
			(obj_.*memFun_)(p1_, p2_);
			dismiss();
		}
	}
protected:
	ObjScopeGuardImpl2(Obj& obj, MemFun memFun, P1 p1, P2 p2) 
		: obj_(obj), memFun_(memFun), p1_(p1), p2_(p2) {}
	Obj& obj_;
	MemFun memFun_;
	const P1 p1_;
	const P2 p2_;
};

template <class Obj, typename MemFun, typename P1, typename P2>
inline ObjScopeGuardImpl2<Obj, MemFun, P1, P2> MakeObjGuard(Obj& obj, MemFun memFun, P1 p1, P2 p2)
{
	return ObjScopeGuardImpl2<Obj, MemFun, P1, P2>::MakeObjGuard(obj, memFun, p1, p2);
}

template <typename Ret, class Obj1, class Obj2, typename P1a, typename P1b, typename P2a, typename P2b>
inline ObjScopeGuardImpl2<Obj1, Ret(Obj2::*)(P1a, P2a), P1b, P2b>
MakeGuard(Ret(Obj2::*memFun)(P1a, P2a), Obj1 &obj, P1b p1, P2b p2)
{
	return ObjScopeGuardImpl2<Obj1, Ret(Obj2::*)(P1a, P2a), P1b, P2b>::MakeObjGuard(obj, memFun, p1, p2);
}



#define ON_BLOCK_EXIT 		ScopeGuard XS_ANONYMOUS_VARIABLE(_scopeGuard__) = MakeGuard

#define ON_BLOCK_EXIT_OBJ 	ScopeGuard XS_ANONYMOUS_VARIABLE(_scopeGuard__) = MakeObjGuard


template <typename T>
void delete_ptr(T* p)
{
	if (p)
	{
		delete p;
	}
}

template <typename T>
void delete_array(T* p)
{
	if (p)
	{
		delete[] p;
	}
}


template <typename T>
void free_pptr(T** pp)
{
	if (*pp)
	{
		free(*pp);
		*pp = NULL;
	}
}

template <typename T>
void delete_pptr(T** pp)
{
	if (*pp)
	{
		delete *pp;
		*pp = NULL;
	}
}

template <typename T>
void delete_parray(T** pp)
{
	if (*pp)
	{
		delete[] *pp;
		*pp = NULL;
	}
}

#endif //SCOPEGUARD_H_
