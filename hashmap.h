#pragma once

#include <functional>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:
    static const int dib_base = -1;
    size_t init_size = 3;

    class HashNode{
    public:
        HashNode(int dib, std::pair<KeyType,ValueType> data, bool empty = false){
            dib_ = dib;
            empty_ = empty;
            data_ = data;
        }

        HashNode(){
            dib_ = dib_base;
            data_ = {KeyType(),ValueType()};
            empty_= true;
        }

        bool get_empty() const{
            return empty_;
        }

        int get_dib() const{
            return dib_;
        }

        const std::pair<KeyType,ValueType>& get_data() const {
            return data_;
        }

        const std::pair<KeyType,ValueType> get_data_val() const {
            return data_;
        }

        std::pair<KeyType,ValueType>& get_data() {
            return data_;
        }

        void set_empty(bool empty){
            empty_ = empty;
/*            dib_ = dib_base;
            data_ = std::pair<KeyType,ValueType>();*/
        }
    private:
        int dib_ = dib_base;
        bool empty_ = true;
        std::pair<KeyType,ValueType> data_;
    };

    class iterator {
    public:
        explicit iterator(const HashNode* pointer){
            pointer_ = const_cast<HashNode*>(pointer);
        }

        iterator(){
            pointer_ = nullptr;
        }

        std::pair<const KeyType, ValueType>& operator*() const{
            return *(operator->());
        }
        std::pair<const KeyType, ValueType>* operator->() const{
            return reinterpret_cast<std::pair<const KeyType, ValueType>*>(&(*pointer_).get_data());
        }

        iterator& operator++(){
            pointer_++;
            while(pointer_->get_empty()){
                pointer_++;
            }
            return *this;
        }
        iterator operator++(int){
            iterator this_it = *this;
            pointer_++;
            while(pointer_->get_empty()){
                pointer_++;
            }
            return this_it;
        }

        bool isEmpty() const {
            return pointer_->get_empty();
        }

        bool operator==(const iterator& other) const{
            return pointer_ == other.pointer_;
        }

        bool operator!=(const iterator& other) const{
            return pointer_ != other.pointer_;
        }

        HashNode& get_pointer(){
            return *pointer_;
        }
    private:
        HashNode* pointer_;
    };



    class const_iterator {
    public:
        explicit const_iterator(const HashNode* pointer){
            pointer_ = const_cast<HashNode*>(pointer);
        }

        const_iterator(){
            pointer_ = nullptr;
        }

        const std::pair<const KeyType, ValueType>& operator*() const{
            return *(operator->());
        }

        const std::pair<const KeyType, ValueType>* operator->() const{
            return reinterpret_cast<std::pair<const KeyType, ValueType>*>(&(*pointer_).get_data());
        }

        bool isEmpty() const {
            return pointer_->get_empty();
        }

        const_iterator& operator++(){
            pointer_++;
            while(pointer_->get_empty()){
                pointer_++;
            }
            return *this;
        }

        const_iterator operator++(int){
            const_iterator this_it = *this;
            pointer_++;
            while(pointer_->get_empty()){
                pointer_++;
            }
            return this_it;
        }

        bool operator==(const const_iterator& other) const{
            return pointer_ == other.pointer_;
        }

        bool operator!=(const const_iterator& other) const{
            return pointer_ != other.pointer_;
        }
    private:
        HashNode* pointer_;
    };

    HashMap(iterator iterator_begin, iterator iterator_end, Hash hash = Hash()):hasher_(hash){
        hash_.assign(init_size, HashNode());
        hash_.push_back(HashNode(dib_base,{KeyType(),ValueType()}, false));
        size_ = (hash_).size();
        for(auto it = iterator_begin; it != iterator_end; it++){
            insert({it->first,it->second});
        }
    }

    HashMap(const_iterator iterator_begin, const_iterator iterator_end, Hash hash = Hash()):hasher_(hash) {
        hash_.assign(init_size, HashNode());
        hash_.push_back(HashNode(dib_base,{KeyType(),ValueType()}, false));
        size_ = (hash_).size();
        for (auto it = iterator_begin; it != iterator_end; it++) {
            insert({it->first,it->second});
        }
    }


    HashMap(const std::initializer_list<std::pair<KeyType, ValueType> >& list, Hash hash = Hash()):hasher_(hash){
        hash_.assign(init_size, HashNode());
        hash_.push_back(HashNode(dib_base,{KeyType(),ValueType()}, false));
        size_ = (hash_).size();
        for(auto el:list){
            insert(el);
        }
    }

    HashMap(Hash hash = Hash()):hasher_(hash) {
        hash_.assign(init_size, HashNode());
        hash_.push_back(HashNode(dib_base,{KeyType(),ValueType()}, false));
        size_ = (hash_).size();
    }

    Hash hash_function() const {
        return hasher_;
    }

    size_t size() const {
        return cnt_nodes_;
    }

    bool empty() const {
        return cnt_nodes_ == 0;
    }

    ValueType& operator[](size_t key){
        //std::cout<<"START []"<<std::endl;
        iterator it = find(key);
        //std::cout<<"FIND []"<<std::endl;
        if(it == end()){
            insert({key,ValueType()});
            //std::cout<<"END1 [] "<<key<<std::endl;
            return (find(key)->second);
        }
        return it->second;
    }

    const ValueType &at(KeyType key) const {
        const_iterator it = find(key);
        if(it == end()){
            find(key);
            throw std::out_of_range("test65665");
        }
        return it->second;
    }

    void insert(std::pair<KeyType,ValueType> data){
        KeyType key = data.first;
        ValueType value = data.second;
        size_t id = hasher_(key)%(hash_.size()-1);
        int dib = 0;
        bool empty = false;

        iterator it = find(key);
        if(it!=end()){
            return;
        }

        while(true){
            if(dib == -1 || empty){
                break;
            }

            if(hash_[id].get_dib() < dib){
                int dib2 = hash_[id].get_dib();
                KeyType key2 = hash_[id].get_data().first;
                ValueType value2 = hash_[id].get_data().second;
                bool empty2 = hash_[id].get_empty();
                hash_[id] = HashNode(dib, {key,value},empty);
                dib = dib2;
                value = value2;
                key = key2;
                empty = empty2;
            }

            dib++;
            id = (id+1)%(hash_.size()-1);
        }

        cnt_nodes_++;

        if(2*cnt_nodes_ >= hash_.size()){
            rebuild();
        }
    }

    void erase(KeyType key){
        iterator it = find(key);

        if(it == end()){
            return;
        }

        it.get_pointer().set_empty(true);
        cnt_nodes_--;

        if(cnt_nodes_*20 < hash_.size()){
            rebuild();
        }
    }

    void rebuild(){
        std::vector<std::pair<KeyType, ValueType> > nodes;
        for(auto it:hash_){
            if(!it.get_empty()){
                nodes.push_back(it.get_data_val());
            }
        }

        size_ = (nodes.size()*10+10);
        cnt_nodes_ = 0;

        nodes.pop_back();
        hash_.assign(size_, HashNode());
        hash_.push_back(HashNode(dib_base,{KeyType(),ValueType()}, false));
        size_ = (hash_).size();

        for(auto it:nodes){
            insert(it);
        }
    }

    iterator find(KeyType key){
        size_t id = hasher_(key)%(hash_.size()-1);
        int dib = 0;

        while(true) {
            if (hash_[id].get_dib() < dib) {
                return end();
            } else if (hash_[id].get_data().first == key && !hash_[id].get_empty()) {
                iterator it = iterator(hash_.data()+id);
                return it;
            }

            dib++;
            id = (id+1)%(hash_.size()-1);
        }
    }

    const_iterator find(KeyType key) const {
        size_t id = hasher_(key)%(hash_.size()-1);
        int dib = 0;

        while(true) {
            if (hash_[id].get_dib() < dib) {
                return end();
            } else if (hash_[id].get_data().first == key && !hash_[id].get_empty()) {
                const_iterator it = const_iterator(hash_.data()+id);
                return it;
            }

            dib++;
            id = (id+1)%(hash_.size()-1);
        }
    }

    void clear(){
        cnt_nodes_ = 0;
        hash_.assign(init_size, HashNode());
        hash_.push_back(HashNode(dib_base,{KeyType(),ValueType()}, false));
        size_ = (hash_).size();
    }

    iterator begin(){
        iterator it = iterator(hash_.data());
        if(it.isEmpty()){
            it++;
        }
        return it;
    }
    iterator end() {
        return iterator(hash_.data()+hash_.size()-1);
    }

    const_iterator begin() const {
        const_iterator it = const_iterator(hash_.data());
        if(it.isEmpty()){
            it++;
        }
        return it;
    }

    const_iterator end() const {
        return const_iterator(hash_.data()+hash_.size()-1);
    }

private:
    std::vector<HashNode> hash_ = std::vector<HashNode>();
    Hash hasher_ = Hash();
    size_t size_ = 0;
    size_t cnt_nodes_ = 0;
};
