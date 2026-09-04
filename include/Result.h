//
// Created by Nemesis Verstraete on 16/08/2026.
//

#pragma once
#include <stdexcept>
#include <variant>

namespace renderium {

template<typename V, typename E>
class Result {
public:
    static Result ok(V value) { return Result(std::move(value)); }
    static Result err(E error) { return Result(std::move(error)); }

    bool isOk() const { return data.index() == 0; }

    bool operator==(const Result &) const {
        return isOk();
    }

    V unwrap() {
        if (!isOk()) throw std::runtime_error("tried to unwrap result without a value!");
        return std::move(std::get<0>(data));
    }

    E unwrapError() {
        if (isOk()) throw std::runtime_error("tried to unwrap error without a value!");
        return std::move(std::get<1>(data));
    }
private:
    explicit Result(V value) : data(std::move(value)) {}
    explicit Result(E error) : data(std::move(error)) {}
    std::variant<V, E> data;
};

}
