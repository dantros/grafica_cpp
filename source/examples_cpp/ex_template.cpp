
#include <iostream>
#include <vector>

template <typename Type>
struct PointT
{
    Type x, y;
};

using Pointi = PointT<int>;
using Pointf = PointT<float>;
using Pointd = PointT<double>;

template <typename Type>
PointT<Type> translate(const PointT<Type>& point, const PointT<Type>& offset)
{
    return {point.x + offset.x, point.y + offset.y};
}

using PointdContainer = std::vector<Pointd>;

template <typename Type>
std::ostream& operator<<(std::ostream& os, const PointT<Type>& point)
{
    os << "(" << point.x << ", " << point.y << ")";
    return os;
}

int main()
{
    Pointf const originalPoint{1,1};
    Pointf const offset{2,0};

    auto const newPoint = translate(originalPoint, offset);

    std::cout << "newPoint = (" << newPoint.x << ", " << newPoint.y << ")" << std::endl;

    PointdContainer points
    {
        {0,0},{10,0},{10,10},{0,10}
    };

    PointdContainer newPoints;

    // this way, we prevent re-allocations when adding new elements
    newPoints.reserve(points.size());

    Pointd offsetd{offset.x, offset.y};

    for (auto& point : points)
    {
        auto const translatedPoint = translate(point, offsetd);
        newPoints.push_back(translatedPoint);
    }

    for (auto& point : newPoints)
    {
        std::cout << point << std::endl;
    }
}
