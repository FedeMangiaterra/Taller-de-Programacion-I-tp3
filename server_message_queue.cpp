#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <utility>
#include <string>
#include <atomic>
#include <condition_variable>
#include "server_message_queue.h"

Message_queue::Message_queue() : queue(new std::deque<std::string>) {}

void Message_queue::push(std::string command) {
    std::unique_lock<std::mutex> lock(this->m);
    this->queue->push_back(command);
    this->queue_condition_variable.notify_all();
}

std::string Message_queue::front() {
    std::unique_lock<std::mutex> lock(this->m);
    return this->queue->front();
}

std::string Message_queue::front_and_pop() {
    std::unique_lock<std::mutex> lock(this->m);
    std::string message = this->queue->front();
    this->queue->pop_front();
    return message;
}

std::string Message_queue::wait_front_pop() {
    std::unique_lock<std::mutex> lock(this->condition_variable_m);
    while (this->empty()) {
        this->queue_condition_variable.wait(lock);
    }
    return this->front_and_pop();
}

size_t Message_queue::size() {
    std::unique_lock<std::mutex> lock(this->m);
    return this->queue->size();
}

bool Message_queue::empty() {
    std::unique_lock<std::mutex> lock(this->m);
    return this->queue->empty();
}

Message_queue::~Message_queue() {
    delete this->queue;
}
