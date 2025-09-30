#ifndef PERSONS_HPP
#define PERSONS_HPP

#include <iostream>
#include <vector>

class Person
{
public:
    Person(int id, int nextId) : mId(id), mNextId(nextId)
    {
        std::cout << "Creating person with id: " << mId << "\n";
    }
private:
    int mId;
    int mNextId;
};

class Persons
{
public:
    Persons(std::string containerName) : mContainerName(containerName)
    {
        std::cout << "Created persons container with name: " << mContainerName << "\n";
    }
    void add(Person person)
    {
        std::unique_lock<std::mutex> lock(mMtx);
        mPersonsVec.push_back(std::move(person));
        std::cout << "Added new person. Container  " << mContainerName << " has new size" << mPersonsVec.size() << "\n";
    }
    void removePerson()
    {
        if (mPersonsVec.size() > 0)
        {
            mPersonsVec.erase(mPersons.begin());  
        }
        else
        {
        }
    }
private:
    std::string mContainerName;
    std::mutex mMtx;
    std::vector<Person> mPersonsVec;
};
#define
