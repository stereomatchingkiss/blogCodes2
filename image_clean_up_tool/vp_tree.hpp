#ifndef VP_TREE_HPP
#define VP_TREE_HPP

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <queue>
#include <limits>

// A VP-Tree implementation, by Steve Hanov. (steve.hanov@gmail.com)
// Released to the Public Domain
// Based on "Data Structures and Algorithms for Nearest Neighbor Search" by Peter N. Yianilos

template<typename T, typename Distance>
class vp_tree
{
public:
    template<typename DistType>
    explicit vp_tree(DistType &&distance) :
        distance_(std::forward<DistType>(distance)),
        root_(nullptr)
    {}

    vp_tree() :
        root_(nullptr)
    {}

    ~vp_tree() {
        delete root_;
    }

    void create(std::vector<T> const &items)
    {
        create_impl(items);
    }

    void create(std::vector<T> &&items)
    {
        create_impl(std::move(items));
    }

    std::vector<T>& get_items()
    {
        return items_;
    }

    std::vector<T> const& get_items() const
    {
        return items_;
    }

    void search( const T& target, int sample_size,
                 std::vector<T> &results,
                 std::vector<double> &distances) const
    {
        search(target, sample_size, results, distances,
               [](auto){return true;});
    }

    template<typename ValidDist>
    void search( const T& target, int sample_size,
                 std::vector<T> &results,
                 std::vector<double> &distances,
                 ValidDist &&valid_dist) const
    {
        std::priority_queue<HeapItem> heap;

        double tau = std::numeric_limits<double>::max();
        search(root_, target, sample_size, heap, tau);

        results.clear(); distances.clear();

        while(!heap.empty()) {
            if(valid_dist(heap.top().dist)){
                results.push_back( items_[heap.top().index] );
                distances.push_back( heap.top().dist );
            }
            heap.pop();
        }

        std::reverse( std::begin(results), std::end(results) );
        std::reverse( std::begin(distances), std::end(distances) );
    }

private:
    struct Node
    {
        size_t index;
        double threshold;
        Node* left;
        Node* right;

        Node() :
            index(0), threshold(0.), left(nullptr), right(nullptr) {}

        ~Node() {
            delete left;
            delete right;
        }
    };

    struct HeapItem {
        HeapItem( size_t index, double dist) :
            index(index), dist(dist) {}
        size_t index;
        double dist;
        bool operator<( const HeapItem& o ) const {
            return dist < o.dist;
        }
    };

    struct DistanceComparator
    {
        Distance &distance_;
        const T& item;
        DistanceComparator(Distance &distance, const T& item ) :
            distance_(distance),
            item(item) {}
        bool operator()(const T& a, const T& b) {
            return distance_( item, a ) < distance_( item, b );
        }
    };

    template<typename U>
    void create_impl(U &&items)
    {
        delete root_;
        items_ = std::forward<U>(items);
        root_ = buildFromPoints(0, items_.size());
    }

    Node* buildFromPoints( size_t lower, size_t upper )
    {
        if ( upper == lower ) {
            return nullptr;
        }

        Node* node = new Node;
        node->index = lower;

        if ( upper - lower > 1 ) {

            // choose an arbitrary point and move it to the start
            size_t const i = (size_t)((double)rand() / RAND_MAX * (upper - lower - 1) ) + lower;
            std::swap( items_[lower], items_[i] );

            size_t const median = ( upper + lower ) / 2;

            // partitian around the median distance
            std::nth_element(
                        items_.begin() + lower + 1,
                        items_.begin() + median,
                        items_.begin() + upper,
                        DistanceComparator(distance_, items_[lower]));

            // what was the median?
            node->threshold = distance_( items_[lower], items_[median] );

            node->index = lower;
            node->left = buildFromPoints( lower + 1, median );
            node->right = buildFromPoints( median, upper );
        }

        return node;
    }

    void search( Node* node, const T& target, size_t k,
                 std::priority_queue<HeapItem>& heap, double &_tau ) const
    {
        if ( node == nullptr ) return;

        double dist = distance_( items_[node->index], target );
        //printf("dist=%g tau=%gn", dist, _tau );

        if ( dist < _tau ) {
            if ( heap.size() == k ) heap.pop();
            heap.push( HeapItem(node->index, dist) );
            if ( heap.size() == k ) _tau = heap.top().dist;
        }

        if ( node->left == nullptr && node->right == nullptr ) {
            return;
        }

        if ( dist < node->threshold ) {
            if ( dist - _tau <= node->threshold ) {
                search( node->left, target, k, heap, _tau );
            }

            if ( dist + _tau >= node->threshold ) {
                search( node->right, target, k, heap, _tau );
            }

        } else {
            if ( dist + _tau >= node->threshold ) {
                search( node->right, target, k, heap, _tau );
            }

            if ( dist - _tau <= node->threshold ) {
                search( node->left, target, k, heap, _tau );
            }
        }
    }

    Distance distance_;
    std::vector<T> items_;
    Node *root_;

};

#endif // VP_TREE_HPP
