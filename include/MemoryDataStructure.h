#ifndef _MemoryDataStructure_h_
#define _MemoryDataStructure_h_

#include "MemoryDatum.h"

class MemoryDataStructure
{
public:
    class DataNode
    {
    private:
        MemoryDatum _data;
        DataNode* _next = nullptr;
        DataNode* _previous = nullptr;

        void setNext(DataNode* next_node) { this->_next = next_node;  }
        void setPrevious(DataNode* previous_node) { this->_previous = previous_node; }
    public:
        DataNode(MemoryDatum data) { this->_data = data; }
        DataMarkers dataMarker() { return this->_data.marker(); }

        DataNode* insertAfter(DataNode* to_insert) 
        { 
            to_insert->setPrevious(this);

            if(this->isTail())
            {
                this->setNext(to_insert);
                return to_insert;
            }

            this->_next->setPrevious(to_insert);
            to_insert->setNext(this->_next);
            this->setNext(to_insert);
            
            return to_insert;
        }

        void insertBefore(DataNode* to_insert) 
        { 
            to_insert->setNext(this);

            if(this->isHead())
            {
                this->setPrevious(to_insert);
                return;
            }

            this->_previous->setNext(to_insert);
            to_insert->setPrevious(_previous);
            this->setPrevious(to_insert);
        }

        void cut()
        {
            if(this->isHead())
            {
                if(!this->isTail())
                    this->_next->setPrevious(nullptr);

                this->setNext(nullptr);
                return;
            }
            if(this->isTail())
            {
                this->_previous->setNext(nullptr);
                this->setPrevious(nullptr);
                return;
            }

            this->_next->setPrevious(_previous);
            this->_previous->setNext(_next);
            this->setPrevious(nullptr);
            this->setNext(nullptr);
        }

        DataNode* next() { return this->_next; }
        DataNode* previous() { return this->_previous; }
        bool isForMarker(DataMarkers marker) { return this->_data.marker() == marker; }

        MemoryDatum* data() { return &this->_data; }
        bool isHead() { return this->_previous == nullptr; }
        bool isTail() { return this->_next == nullptr; }
        bool hasNext() { return this->_next != nullptr; }
    };

    class MemoryDataIterator
    {
     private:
        DataNode* _head = nullptr;
        DataNode* _current = nullptr;
     public:
        MemoryDataIterator(DataNode* head){ this->_head = this->_current = head; }
        bool isValid() { return this->_current != nullptr; }
        DataNode* current() { return this->_current; }
        MemoryDatum* current_data() { return this->_current->data(); }
        void next() { _current = _current->next(); }
    };

    class MemoryDataReverseIterator
    {
     private:
        DataNode* _tail = nullptr;
        DataNode* _current = nullptr;
     public:
        MemoryDataReverseIterator(DataNode* tail){ this->_tail = this->_current = tail; }
        bool isValid() { return this->_current != nullptr; }
        DataNode* current() { return this->_current; }
        MemoryDatum* current_data() { return this->_current->data(); }
        void next() { this->_current = this->_current->previous(); }
    };

private:
    DataNode* _head = nullptr;
    DataNode* _tail = nullptr;
    int _length = 0;

    DataNode* findByMarker(DataMarkers marker)
    { 
        DataNode* find_result = this->_head;

        while(find_result != nullptr) 
        {
            if(find_result->isForMarker(marker))
            {
                break;
            }
            find_result = find_result->next();
        } 

        return find_result;
    }
    DataNode* _foundNode = nullptr;
    DataNode* head() { return this->_head; }
    DataNode* tail() { return this->_tail; }
public:
    int length() { return this->_length; }
    bool find(DataMarkers marker) 
    { 
        this->_foundNode = findByMarker(marker);       
        return this->_foundNode != nullptr;
    }

    MemoryDatum* findResult() { return this->_foundNode->data(); }

    void add(MemoryDatum new_data)
    {
        if(this->head() == nullptr)
        {
            this->_head = new DataNode(new_data);
            this->_tail = this->_head;
            return;
        }

        this->_tail = this->_tail->insertAfter(new DataNode(new_data));
        this->_length += 1;
    }

    void remove(DataMarkers marker)
    {
        DataNode* to_remove = this->findByMarker(marker);
        if(to_remove->isHead()) { _head = to_remove->next(); }
        if(to_remove->isTail()) { _tail = to_remove->previous(); }
        to_remove->cut();
        delete to_remove;
        this->_length -= 1;
    }

    void clear()
    {
        while(_head != nullptr)
        {
            this->remove(_head->dataMarker());
        }
    }

    MemoryDataStructure::MemoryDataIterator iterator() { return MemoryDataIterator(this->_head); }
    MemoryDataStructure::MemoryDataReverseIterator reverse_iterator() { return MemoryDataReverseIterator(this->_tail); }
};

#endif