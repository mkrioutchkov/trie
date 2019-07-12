// Wandbox dump for now
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

template<typename, typename>
struct match_ref;

template<typename T, typename OTHER>
struct match_ref<T, OTHER&>
{
    using type = T&;
};

template<typename T, typename OTHER>
struct match_ref<T, OTHER&&>
{
    using type = T&&;
};

template<typename T, typename OTHER>
struct match_ref<T, const OTHER&>
{
    using type = const T&;
};

template<typename T, typename OTHER>
struct match_ref<T, const OTHER&&>
{
    using type = const T&&;
};

template<typename T, typename OTHER>
using match_ref_t = typename match_ref<T, OTHER>::type;

struct default_trie_adaptation
{
    static constexpr size_t CONTAINER_OFFSET_START = 0;
    static constexpr size_t CONTAINER_OFFSET_END = 0;
};

struct string_trie_adaptation : default_trie_adaptation
{
    static constexpr size_t CONTAINER_OFFSET_END = 1;
};

template<typename T, typename TRIE_ADAPTATION = default_trie_adaptation>
struct trie
{
    struct iterator
    {
        // todo
    };
    
    auto insert(T&& value)
    {
        m_root[std::forward<T>(value)];
    }
    
    template<typename CONTAINER>
    auto insert(CONTAINER&& container)
    {
        using FWD_T = match_ref_t<T, CONTAINER&&>;
        auto* currentPosition = &m_root;
        for (auto& element : container)
            currentPosition = &currentPosition->operator[](static_cast<FWD_T>(element));
    }
    
    template<typename IT>
    auto mismatch(const IT& begin, const IT& end) const
    {
        auto* currentPosition = &m_root;
        for (auto mismatchIt = begin; mismatchIt != end; ++mismatchIt)
        {
            if (auto it = currentPosition->find(*mismatchIt); it != std::end(*currentPosition))
                currentPosition = &it->second;
            else
                return mismatchIt;
        }
        return end;
    }
    
    template<typename CONTAINER>
    auto mismatch(const CONTAINER& container) const
    {
        const auto begin = std::next(std::begin(container), TRIE_ADAPTATION::CONTAINER_OFFSET_START);
        const auto end = std::prev(std::end(container), TRIE_ADAPTATION::CONTAINER_OFFSET_END);
        return mismatch(begin, end);
    }
    
    template<typename IT>
    auto contains(const IT& begin, const IT& end) const
    {
        return mismatch(begin, end) == end;
    }
    
    template<typename CONTAINER>
    auto contains(const CONTAINER& container) const
    {
        const auto begin = std::next(std::begin(container), TRIE_ADAPTATION::CONTAINER_OFFSET_START);
        const auto end = std::prev(std::end(container), TRIE_ADAPTATION::CONTAINER_OFFSET_END);
        return contains(begin, end);
    }
private:
    struct node_map_t;
    struct node_map_t : std::unordered_map<T, node_map_t>
    {
        using std::unordered_map<T, node_map_t>::unordered_map;
    };
    node_map_t m_root;
};

#include <iostream>
#include <cstdlib>
#include <string>

int main()
{
    trie<char, string_trie_adaptation> string_trie;
    string_trie.insert("string (literal)");
    string_trie.insert(std::string("string (std::string)"));
    std::cout << string_trie.contains("") << std::endl;
    std::cout << string_trie.contains("") << std::endl;
    std::cout << string_trie.contains("string (") << std::endl;
    std::cout << string_trie.contains("str") << std::endl;
    std::cout << string_trie.contains("string (std::") << std::endl;
    std::cout << string_trie.contains("string (literal)") << std::endl;
    std::cout << string_trie.contains("string (literal)s") << std::endl;
    std::cout << string_trie.contains("string (literal)s") << std::endl;
    std::cout << string_trie.contains("nonsense") << std::endl;
    std::cout << string_trie.contains("string (something") << std::endl;
    std::cout << *string_trie.mismatch("stxyz") << std::endl;
}
