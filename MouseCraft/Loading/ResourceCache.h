#pragma once

#include <map>
#include <string>
#include <iostream>



// Simple manager that allows retrieval of type with string key
template<class T>
class ResourceCache
{
// singleton 
public:
	static ResourceCache& Instance()
	{
		static ResourceCache<T> _instance;
		return _instance;
	}
	ResourceCache(ResourceCache const&) = delete;
	void operator=(ResourceCache const&) = delete;
private:
	ResourceCache() {};
	~ResourceCache() {};

public:
	T* Get(std::string key)
	{
		if (_cache.find(key) != _cache.end())
		{
			return _cache[key];
		}
		else
		{
			return nullptr;
		}
	}

	void Add(std::string key, T* value)
	{
		_cache[key] = value;
	}
	
	void Clear()
	{
		for (auto& kvp : _cache)
			delete(kvp.second);
		_cache.clear();
	}

private:
	std::map<std::string, T*> _cache;
};



// Like ResourceCache but with SharedPointers 
template<class T>
class ResourceCacheShared
{
	// singleton 
public:
	static ResourceCacheShared& Instance()
	{
		static ResourceCacheShared<T> _instance;
		return _instance;
	}
	ResourceCacheShared(ResourceCacheShared const&) = delete;
	void operator=(ResourceCacheShared const&) = delete;
private:
	ResourceCacheShared() {};
	~ResourceCacheShared() {};

public:
	std::shared_ptr<T> Get(std::string key)
	{
		if (_cache.find(key) != _cache.end())
		{
			return _cache[key];
		}
		else
		{
			return nullptr;
		}
	}

	void Add(std::string key, std::shared_ptr<T> ptr)
	{
		_cache[key] = ptr;
	}

	template<typename... Args>
	void Add(std::string key, Args... args)
	{
		_cache[key] = std::make_shared(args...);
	}

	void Clear()
	{
		_cache.clear();
	}

private:
	std::map<std::string, std::shared_ptr<T>> _cache;
};