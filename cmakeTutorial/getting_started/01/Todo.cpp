#include "ToDo.h"


ToDo::ToDo()
{
}

ToDo::~ToDo()
{
}


size_t ToDo::size() const
{
    return this_tasks.size();
}


void ToDo::addTask(
    const std::string& task
)
{
    this_tasks.push_back(task);
}

std::string ToDo::getTask(
    size_t index
) const
{
    if (index < this_tasks.size())
    {
        return this_tasks[index];
    }
    else
    {
        return "";
    }
}
