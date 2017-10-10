#pragma once

#include <functional>
#include "future.h"

template<typename T>
class Promise {

public:

    Promise() : state_(std::make_shared<FutureState<T> >()), futureExist_(false) {
        state_->hasPromise = true;
    }

    Promise(Promise<T> &&promise) : state_(std::move(promise.state_)) {
        state_->hasPromise=true;
    }
    ~Promise()
    {
        if(state_) {
            state_->hasPromise = false;
            state_->cv.notify_one();
        }
    }   
    Promise &operator=(Promise<T> &&promise) {
        state_ = std::move(promise.state_);
        return *this;
    };

    Promise(Promise<T> const &) = delete;

    Promise &operator=(Promise<T> const &) = delete;

    Future<T> getFuture() {
        if (futureExist_) {
            throw std::runtime_error("future already set");
        }
        futureExist_ = true;
        return Future<T>(state_);
    }

    void set(const T &v) {
        std::unique_lock<std::mutex> lock(state_->mx);
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->value = v;
        state_->isReady = true;
        state_->cv.notify_one();

    }

    void set(T &&v) {
        std::unique_lock<std::mutex> lock(state_->mx);
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->value = std::move(v);
        state_->isReady = true;
        state_->cv.notify_one();
    }

    void setException(const std::exception_ptr &e) {
        std::unique_lock<std::mutex> lock(state_->mx);
        if (state_->except) {
            throw std::runtime_error("error already set");
        }
        state_->except = e;
        state_->isReady = true;
        state_->cv.notify_one();
    }


private:
    std::shared_ptr<FutureState<T> > state_;
    std::atomic<bool> futureExist_;

};

template<>
class Promise<void> {
public:

    Promise()
            : state_(std::make_shared<FutureState<void> >()), futureExist_(false) {
        state_->hasPromise = true;
    }

    Promise(Promise<void> &&promise) : state_(std::move(promise.state_)) {
    }

    Promise &operator=(Promise<void> &&promise) { // incorrect
        state_ = std::move(promise.state_);
        return *this;
    };

    Promise(Promise<void> const &) = delete;

    Promise &operator=(Promise<void> const &) = delete;

    Future<void> getFuture() {
        if (futureExist_) {
            throw std::runtime_error("future already set");
        }
        futureExist_ = true;
        return Future<void>(state_);
    }

    void set() {
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->isReady = true;
        state_->cv.notify_one();
    }

    void setException(const std::exception_ptr &e) {
        std::unique_lock<std::mutex> lock(state_->mx);
        if (state_->except) {
            throw std::runtime_error("error already set");
        }
        state_->except = e;
        state_->isReady = true;
        state_->cv.notify_one();
    };


private:
    std::shared_ptr<FutureState<void> > state_;
    bool futureExist_;
};

template<typename T>
class Promise<T &> {

public:

    Promise()
            : state_(std::make_shared<FutureState<T &> >())
            , futureExist_(false) {
        state_->hasPromise = true;
    }

    Promise(Promise &&promise) : state_(std::move(promise.state_)) {
    }

    Promise &operator=(Promise &&promise) {
        state_ = std::move(promise.state_);
        return *this;
    };

    Promise(const Promise &) = delete;

    Promise &operator=(const Promise &) = delete;

    Future<T &> getFuture() {
        if (futureExist_) {
            throw std::runtime_error("future already set");
        }
        futureExist_ = true;
        return Future<T &>(state_);
    }

    void set(T &v) {
        std::unique_lock<std::mutex> lock(state_->mx);
        if (state_->isReady) {
            throw std::runtime_error("value already set");
        }
        state_->value = &v;
        state_->isReady = true;
        state_->cv.notify_one();

    }

    void setException(const std::exception_ptr &e) {
        std::unique_lock<std::mutex> lock(state_->mx);
        if (state_->except) {
            throw std::runtime_error("error already set");
        }
        state_->except = e;
        state_->isReady = true;
        state_->cv.notify_one();
    }


private:
    std::shared_ptr<FutureState<T &> > state_;
    bool futureExist_;

};




