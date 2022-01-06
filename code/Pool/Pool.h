#pragma once
#include <vector>

/// <summary>
/// This is done so we can create Pools without necessarily needing to know
/// the type to create them beforehand.
/// </summary>
struct IPool {
public:
	virtual ~IPool() = 0 {};
};

template <typename T> 
/// <summary>
/// A vector (contiguous data) of objects of type T.
/// </summary>
/// <typeparam name="T">type of objects the pool is containing</typeparam>
class Pool : public IPool
{
private:
	std::vector<T> data;

public:
	/// <summary>
	/// Initializes the pool
	/// </summary>
	/// <param name="size">Size of the pool</param>
	Pool(int size = 100) { data.resize(100); }
	virtual ~Pool() = default;
	bool isEmpty() const {
		return data.empty();
	}
	int GetSize() const {
		return static_cast<int>(data.size());
	}
	void Resize(int n) {
		data.resize(n);
	}
	void Clear(){
		data.clear();
	}
	void Add(T object) {
		data.push_back(object);
	}
	T& Get(int index) {
		return static_cast<T&>(data[index]);
	}
	void Set(int index, T object)
	{
		data[index] = object;
	}
	std::vector<T> operator[](unsigned int index) {
		return data[index];
	}
};