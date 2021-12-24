#include <iostream>
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::cout;
using std::cin;

class BigInteger {
  private:
    const size_t COUNT = 9;
    const size_t SIZE = 1e9;
    bool is_positive_ = true;
    vector<size_t> number_;
    size_t size_ = 0;
    size_t rank_ = 0;
    
    void swap(BigInteger&);
    void fix_number();

  public:
    BigInteger() {}
    BigInteger(int);
    explicit BigInteger(size_t size, size_t rank) : number_(size), size_(size), rank_(rank) {}
    BigInteger(const BigInteger& arg_number) : 
      is_positive_(arg_number.is_positive_), number_(arg_number.number_), size_(arg_number.size_), 
      rank_(arg_number.rank_) {}
    explicit BigInteger(size_t is_positive, const vector<size_t>& number, size_t size, size_t rank) :
      is_positive_(is_positive), number_(number), size_(size), rank_(rank) {}
    explicit BigInteger(const string&);

    operator int() const; // ВНИМАНИЕ! НЕЯВНОЕ ПРЕОБРАЗОВАНИЕ
    operator bool() const { return rank_ || number_[0]; } // ВНИМАНИЕ! НЕЯВНОЕ ПРЕОБРАЗОВАНИЕ
    string toString() const;

    friend BigInteger abs(const BigInteger&);
    friend BigInteger min(const BigInteger&, const BigInteger&);
    friend BigInteger max(const BigInteger&, const BigInteger&);

    BigInteger& operator=(const BigInteger&);
    BigInteger& operator+=(const BigInteger&);
    BigInteger& operator-=(const BigInteger&);
    BigInteger& operator*=(const BigInteger&);
    BigInteger& operator/=(const BigInteger&);
    BigInteger& operator%=(const BigInteger&);

    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);    

    friend BigInteger operator-(const BigInteger&);
    friend bool operator>(const BigInteger&, const BigInteger&);
    friend bool operator==(const BigInteger&, const BigInteger&);
    
    // если не сделать их friend'ом, то они не будут вызываться???
    friend bool operator>=(const BigInteger&, const BigInteger&);
    friend bool operator<=(const BigInteger&, const BigInteger&);
    friend bool operator<(const BigInteger&, const BigInteger&);
    friend bool operator!=(const BigInteger&, const BigInteger&);

    friend std::ostream& operator<<(std::ostream&, const BigInteger&);
    friend std::istream& operator>>(std::istream&, const BigInteger&);
};

BigInteger::BigInteger(int arg_number) : is_positive_(arg_number >= 0), number_(2), size_(2), rank_(0) {
  arg_number = std::abs(arg_number);
  do {
    if (rank_ == size_) {
      size_ *= 2;
      number_.resize(size_);
    }
    number_[rank_] = arg_number % SIZE;
    arg_number /= SIZE;
    if (arg_number != 0) {
      ++rank_;
    }
  } while (arg_number != 0);
}

BigInteger::BigInteger(const string& string_number) : size_(2), rank_(0) {
  if (string_number.size() > 0) {
    is_positive_ = (string_number[0] != '-');
    string current_rank;
    size_t last_pos = string_number.size() - 1;
    while (last_pos >= COUNT) {
      last_pos -= COUNT;
      current_rank = string_number.substr(last_pos + 1, COUNT);
      number_.push_back(stoi(current_rank));
    }
    current_rank = string_number.substr(1 - is_positive_, last_pos + is_positive_);
    number_.push_back(stoi(current_rank));
    rank_ = number_.size() - 1;
    while(size_ * COUNT < string_number.size()) {
      size_ *= 2;
    } 
    number_.resize(size_);
  }
}

BigInteger::operator int() const {
  int result = 0;
  for (size_t current_rank = rank_ + 1; current_rank > 0; --current_rank) {
    result *= SIZE;
    result += number_[current_rank - 1];
  }
  return result;
}

string BigInteger::toString() const {
  string result;
  if (!is_positive_) {
    result += '-';
  }
  for (size_t current_rank = rank_ + 1; current_rank > 0; --current_rank) {
    string string_part = std::to_string(number_[current_rank - 1]);
    size_t leading_zeros_count = COUNT - string_part.size() + 1;
    if (leading_zeros_count > 1 && current_rank != rank_ + 1) {
      while (leading_zeros_count > 1) {
        result += '0';
        --leading_zeros_count;
      }
    }
    result += string_part;
  }
  return result;
}

void BigInteger::swap(BigInteger& arg_number) {
  std::swap(is_positive_, arg_number.is_positive_);
  std::swap(number_, arg_number.number_);
  std::swap(size_, arg_number.size_);
  std::swap(rank_, arg_number.rank_);
}

BigInteger abs(const BigInteger& number) {
  BigInteger temp_number = number;
  temp_number.is_positive_ = true;
  return temp_number;
}

BigInteger min(const BigInteger& first_number, const BigInteger& second_number) {
  return (first_number <= second_number ? first_number : second_number);
}

BigInteger max(const BigInteger& first_number, const BigInteger& second_number) {
  return (first_number >= second_number ? first_number : second_number);
}

BigInteger& BigInteger::operator=(const BigInteger& arg_number) {
  BigInteger temp_number(arg_number);
  swap(temp_number);
  return *this;
}

void BigInteger::fix_number() {
  while (number_[rank_] == 0 && rank_ > 0) {
    --rank_;
  }
  while (size_ > 2 && size_ > 4 * rank_) {
    size_ /= 2;
    number_.resize(size_);
  }
  if (rank_ == 0 && number_[rank_] == 0) {
    is_positive_ = true;
  }
}

BigInteger& BigInteger::operator+=(const BigInteger& second_number) {
  if (is_positive_ == second_number.is_positive_) {
    size_t current_rank = 0, current_sum = 0;
    do {
      if (current_rank == size_) {
        size_ *= 2;
        number_.resize(size_);
      }
      current_sum += number_[current_rank]; 
      if (current_rank <= second_number.rank_) {
        current_sum += second_number.number_[current_rank];
      }
      number_[current_rank] = current_sum % SIZE;
      current_sum /= SIZE;
      rank_ = std::max(rank_, current_rank++);
    } while (current_sum > 0 || current_rank <= second_number.rank_);
  } else {
    if (abs(*this) >= abs(second_number)) {
      bool is_lend = false;
      size_t current_rank = 0;
      while (current_rank <= second_number.rank_) {
        if (number_[current_rank] >= second_number.number_[current_rank] + is_lend) {
          number_[current_rank] -= second_number.number_[current_rank] + is_lend;
          is_lend = false;
        } else {
          number_[current_rank] = SIZE - is_lend + number_[current_rank] - 
            second_number.number_[current_rank];
          is_lend = true;
        }
        ++current_rank;
      }
      while (is_lend) {
        if (number_[current_rank] >= is_lend) {
          number_[current_rank] -= is_lend;
          is_lend = false;
        } else {
          number_[current_rank] = SIZE - is_lend + number_[current_rank]; // по факту, number_[current_rank] = 0;
        }
        ++current_rank;
      }
    } else { 
      is_positive_ = !is_positive_;
      number_.resize(second_number.number_.size());
      size_ = second_number.size_;
      rank_ = second_number.rank_;
      bool is_lend = false;
      for (size_t current_rank = 0; current_rank <= rank_; ++current_rank) {
        if (second_number.number_[current_rank] >= number_[current_rank] + is_lend) {
          number_[current_rank] = second_number.number_[current_rank] - 
            number_[current_rank] - is_lend;
          is_lend = false;
        } else {
          number_[current_rank] = SIZE - is_lend + second_number.number_[current_rank] - 
            number_[current_rank];
          is_lend = true;
        }
      }
    }  
  }
  (*this).fix_number();
  return *this;
}

BigInteger operator+(const BigInteger& first_number, const BigInteger& second_number) {
  BigInteger temp_number(first_number);
  temp_number += second_number;
  return temp_number;
}

BigInteger operator+(const BigInteger& first_number, int second_number) {
  return first_number + BigInteger(second_number);
}

BigInteger operator+(int first_number, const BigInteger& second_number) {
  return BigInteger(first_number) + second_number;
}

BigInteger& BigInteger::operator-=(const BigInteger& second_number) { 
  return (*this += -second_number); 
}

BigInteger operator-(const BigInteger& first_number, const BigInteger& second_number) {
  BigInteger temp_number(first_number);
  temp_number -= second_number;
  return temp_number;
}

BigInteger operator-(const BigInteger& first_number, int second_number) {
  return first_number - BigInteger(second_number);
}

BigInteger operator-(int first_number, const BigInteger& second_number) {
  return BigInteger(first_number) - second_number;
}

BigInteger& BigInteger::operator*=(const BigInteger& second_number) {
  size_t max_size = std::max(size_, second_number.size_) * 2;
  vector<size_t> new_number(max_size, 0);
  for (size_t first_rank = 0; first_rank <= rank_; ++first_rank) {
    for (size_t second_rank = 0; second_rank <= second_number.rank_; ++second_rank) {
      new_number[second_rank + first_rank] += number_[first_rank] * second_number.number_[second_rank];
      new_number[second_rank + first_rank + 1] += new_number[second_rank + first_rank] / SIZE;
      new_number[second_rank + first_rank] %= SIZE;
    }
  }
  is_positive_ = (is_positive_ == second_number.is_positive_);
  number_ = new_number;
  size_ = max_size;
  rank_ = max_size - 1;
  (*this).fix_number();
  return *this;
}

BigInteger operator*(const BigInteger& first_number, const BigInteger& second_number) {
  BigInteger temp_number(first_number);
  temp_number *= second_number;
  return temp_number;
}

BigInteger operator*(const BigInteger& first_number, int second_number) {
  return first_number * BigInteger(second_number);
}

BigInteger operator*(int first_number, const BigInteger& second_number) {
  return BigInteger(first_number) * second_number;
}

BigInteger& BigInteger::operator/=(const BigInteger& second_number) {
  BigInteger result(0), subnumber(0), abs_second_number = abs(second_number);
  size_t current_rank = rank_ + 1;
  while (subnumber < abs_second_number && current_rank > 0) {
    subnumber *= SIZE;
    subnumber += number_[--current_rank];
  }
  while (true) {
    size_t left_value = 0, right_value = SIZE;
    if (subnumber.rank_ - second_number.rank_ == 1) {
      left_value = SIZE / (second_number.number_[second_number.rank_] + 1);
      right_value = SIZE;
    } else if (subnumber.rank_ == second_number.rank_) {
      left_value = subnumber.number_[subnumber.rank_] / 
        (second_number.number_[second_number.rank_] + 1);
      right_value = (subnumber.number_[subnumber.rank_] + 1) / 
        second_number.number_[second_number.rank_];
    }
    ++right_value;
    while (left_value + 1 < right_value) {
      size_t middle_value = (left_value + right_value) >> 1;
      if (abs_second_number * BigInteger(middle_value) <= subnumber) { // почему возникает сообщение от компилятора, если нет явного преобразования? Оператор ведь перегружен для BigInteger * int.
        left_value = middle_value;
      } else {
        right_value = middle_value;
      }
    }
    subnumber -= abs_second_number * BigInteger(left_value);
    result *= SIZE;
    result += left_value;
    if (current_rank > 0) {
      subnumber *= SIZE;
      subnumber += number_[--current_rank];
    } else {
      break;
    }
  }
  result.is_positive_ = (is_positive_ == second_number.is_positive_);
  *this = result;
  (*this).fix_number();
  return *this;
}

BigInteger operator/(const BigInteger& first_number, const BigInteger& second_number) {
  BigInteger temp_number(first_number);
  temp_number /= second_number;
  return temp_number;
}

BigInteger operator/(const BigInteger& first_number, int second_number) {
  return first_number / BigInteger(second_number);
}

BigInteger operator/(int first_number, const BigInteger& second_number) {
  return BigInteger(first_number) / second_number;
}

BigInteger& BigInteger::operator%=(const BigInteger& second_number) {
  *this = *this - (*this / second_number) * second_number;
  return *this;
}

BigInteger operator%(const BigInteger& first_number, const BigInteger& second_number) {
  BigInteger temp_number = first_number;
  temp_number %= second_number;
  return temp_number;
} 

BigInteger operator%(const BigInteger& first_number, size_t second_number) {
  return first_number % BigInteger(second_number);
}

BigInteger operator%(int first_number, const BigInteger& second_number) {
  return BigInteger(first_number) % second_number;
}

BigInteger& BigInteger::operator++() {
  (*this) += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger temp_number(*this);
  ++(*this);
  return temp_number;
}

BigInteger& BigInteger::operator--() {
  (*this) -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger temp_number(*this);
  --(*this);
  return temp_number;
}

BigInteger operator-(const BigInteger& number) {
  if (number != BigInteger(0)) {
    BigInteger temp_number(number);
    temp_number.is_positive_ = !temp_number.is_positive_;
    return temp_number;
  } else {
    return number;
  }
}

bool operator>(const BigInteger& first_number, const BigInteger& second_number) {
  if (first_number.is_positive_ != second_number.is_positive_) {
    return (first_number.is_positive_ == 1 ? true : false);
  } else {
    if (first_number.rank_ != second_number.rank_) {
      return (first_number.is_positive_ ? 
          first_number.rank_ > second_number.rank_ : first_number.rank_ < second_number.rank_);
    } else {
      bool is_more = false;
      for (size_t current_rank = first_number.rank_ + 1; current_rank > 0; --current_rank) {
        if (first_number.number_[current_rank - 1] == second_number.number_[current_rank - 1]) {
          continue;
        } else {
          is_more = first_number.number_[current_rank - 1] > 
            second_number.number_[current_rank - 1];
          return (first_number.is_positive_ ? is_more : !is_more);
        }
      }
      return false;
    }
  }
}

bool operator<(const BigInteger& first_number, const BigInteger& second_number) {
  return second_number > first_number;
}

bool operator==(const BigInteger& first_number, const BigInteger& second_number) {
  if (first_number.rank_ != second_number.rank_ || 
      first_number.is_positive_ != second_number.is_positive_) {
    return false;
  } else {
    for (size_t current_rank = 0; current_rank <= first_number.rank_; ++current_rank) {
      if (first_number.number_[current_rank] != second_number.number_[current_rank]) {
        return false;
      }
    }
    return true;
  }
}

bool operator!=(const BigInteger& first_number, const BigInteger& second_number) {
  return !(first_number == second_number);
}

bool operator>=(const BigInteger& first_number, const BigInteger& second_number) {
  return first_number == second_number || first_number > second_number;
}

bool operator<=(const BigInteger& first_number, const BigInteger& second_number) {
  return second_number >= first_number;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& number) {
  out << number.toString();
  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& number) {
  string string_number;
  in >> string_number;
  number = BigInteger(string_number);
  return in;
}

BigInteger operator "" _bi(const char* string_number) {
  return BigInteger(string_number);
}

class Rational{
  private:
    BigInteger numerator_, denominator_; 
    BigInteger GCD(BigInteger&, BigInteger&);
    void swap(Rational&);
    void fix_number();

  public:
    Rational() : numerator_(0), denominator_(1) {}
    Rational(const Rational& number) : 
      numerator_(number.numerator_), denominator_(number.denominator_) {}
    Rational(int numerator) : numerator_(numerator), denominator_(1) {}
    Rational(const BigInteger& number) : numerator_(number), denominator_(1) {}

    explicit operator double() const;

    string toString()const;
    string asDecimal(size_t) const;
    
    Rational& operator=(const Rational&);
    Rational& operator+=(const Rational&);
    Rational& operator-=(const Rational&);
    Rational& operator*=(const Rational&);
    Rational& operator/=(const Rational&);

    friend Rational operator-(const Rational&);
    friend bool operator>(const Rational&, const Rational&);
    friend bool operator==(const Rational&, const Rational&);

    friend std::istream& operator>>(std::istream&, Rational&);
};

BigInteger Rational::GCD(BigInteger& first_number, BigInteger& second_number) {
  if (second_number == BigInteger(0)) {
    return first_number;
  } else {
    first_number %= second_number;
    return GCD(second_number, first_number);
  }
}

Rational::operator double() const {
  string result = (*this).asDecimal(16); // по факту, лишняя линяя. Но можно сделать, как в asDecimal
  return stod(result);
}

void Rational::swap(Rational& arg_number) {
  std::swap(numerator_, arg_number.numerator_);
  std::swap(denominator_, arg_number.denominator_);
}

void Rational::fix_number() {
  BigInteger first_number = abs(numerator_), 
             second_number = denominator_;
  BigInteger gcd = GCD(first_number, second_number);
  numerator_ /= gcd;
  denominator_ /= gcd;
  if (denominator_ < BigInteger(0)) {
    numerator_ *= -1;
    denominator_ *= -1;
  }
}

string Rational::toString() const {
 string result = numerator_.toString();
 if (denominator_ != BigInteger(1)) {
   result += '/' + denominator_.toString();
 }
 return result;
}

string Rational::asDecimal(size_t precision = 0) const {
  BigInteger quotient = numerator_ / denominator_;
  BigInteger remainder = abs(numerator_ - quotient * denominator_);
  string result;
  if (numerator_ < BigInteger(0)) {
    result += '-';
  }
  result += quotient.toString();
  if (precision != 0) {
    result += '.';
    while (precision-- > 0) {
      remainder *= 10;
      result += (remainder / denominator_).toString();
      remainder %= denominator_;
    }
  }
  return result;
}

Rational& Rational::operator=(const Rational& arg_number) {
  Rational temp_number(arg_number);
  swap(temp_number);
  return *this;
}

Rational& Rational::operator+=(const Rational& second_number) {
  numerator_ = numerator_ * second_number.denominator_ + denominator_ * second_number.numerator_;
  denominator_ *= second_number.denominator_;
  (*this).fix_number();
  return *this;
}

Rational operator+(const Rational& first_number, const Rational& second_number) {
  Rational temp_number(first_number);
  temp_number += second_number;
  return temp_number;
}

Rational& Rational::operator-=(const Rational& second_number) { 
  return (*this += -second_number); 
}

Rational operator-(const Rational& first_number, const Rational& second_number) {
  Rational temp_number(first_number);
  temp_number -= second_number;
  return temp_number;
}

Rational& Rational::operator*=(const Rational& second_number) {
 numerator_ *= second_number.numerator_;
 denominator_ *= second_number.denominator_;
 (*this).fix_number();
 return *this;
}

Rational operator*(const Rational& first_number, const Rational& second_number) {
  Rational temp_number(first_number);
  temp_number *= second_number;
  return temp_number;
}

Rational& Rational::operator/=(const Rational& second_number) {
  numerator_ *= second_number.denominator_; 
  denominator_ *= second_number.numerator_;
  (*this).fix_number();
  return *this;
}

Rational operator/(const Rational& first_number, const Rational& second_number) {
  Rational temp_number(first_number);
  temp_number /= second_number;
  return temp_number;
}

Rational operator-(const Rational& number) {
  Rational temp_number(number);
  temp_number.numerator_ = -temp_number.numerator_;
  return temp_number;
}

bool operator>(const Rational& first_number, const Rational& second_number) {
  BigInteger first_numerator = first_number.numerator_ * second_number.denominator_;
  BigInteger second_numerator = first_number.denominator_ * second_number.numerator_;
  return first_numerator > second_numerator;
}

bool operator<(const Rational& first_number, const Rational& second_number) {
  return second_number > first_number;
}

bool operator==(const Rational& first_number, const Rational& second_number) {
  return first_number.numerator_ == second_number.numerator_ && 
    first_number.denominator_ == second_number.denominator_;
}

bool operator!=(const Rational& first_number, const Rational& second_number) {
  return !(first_number == second_number);
}

bool operator>=(const Rational& first_number, const Rational& second_number) {
  return first_number == second_number || first_number > second_number;
}

bool operator<=(const Rational& first_number, const Rational& second_number) {
  return second_number >= first_number;
}

std::istream& operator>>(std::istream& in, Rational& number) {
  BigInteger temp_number;
  in >> temp_number;
  number = Rational(temp_number);
  return in;
}
//#include "biginteger.h"

template <size_t N>
class Residue {
  private:
    uint64_t value_; 

    struct isPrime;
 
    static Residue<N> getReverseItem(const Residue<N>&);

  public:
    Residue<N>() : value_(0) {}
    Residue<N>(int value) : value_(value > 0 ? value % N : (N + value % int(N)) % N) {}

    explicit operator int() const { return value_; } 
    
    Residue<N>& operator=(const Residue<N>&);
    Residue<N>& operator+=(const Residue<N>&);
    Residue<N>& operator-=(const Residue<N>&);
    Residue<N>& operator*=(const Residue<N>&);
    Residue<N>& operator/=(const Residue<N>&);
    Residue<N>& operator%=(const Residue<N>&);

    template <size_t M>
    friend Residue<M> operator-(const Residue<M>&);

    bool operator==(const Residue<N>& second_item) const { return value_ == second_item.value_; }
    bool operator!=(const Residue<N>& second_item) const { return value_ != second_item.value_; };    
};

template <size_t N>
Residue<N> Residue<N>::getReverseItem(const Residue<N>& item) {
  // Euler's th.
  Residue result(1);
  
  uint32_t degree = N - 2;

  while (degree > 0) {
    if (degree % 2 == 0) {
      degree /= 2;
      result *= result;
    } else {
      --degree;
      result *= item;
    }
  }
  
  return result;
}

template <size_t N>
struct Residue<N>::isPrime { 
  template <uint32_t divisor, bool isEnough = false>
  struct isPrimeHelper {
    static const bool value = (N == 2) || (N % divisor != 0 && 
      isPrimeHelper<divisor + 1, ((divisor + 1) * (divisor + 1) > N)>::value);
  };

  template <uint32_t divisor>
  struct isPrimeHelper<divisor, true> {
    static const bool value = true;
  };

  static const bool result = isPrimeHelper<2>::value;
};

template <size_t N>
Residue<N>& Residue<N>::operator=(const Residue<N>& second_item) {
  value_ = second_item.value_;

  return *this;
}

template <size_t N>
Residue<N>& Residue<N>::operator+=(const Residue<N>& second_item) {
  value_ += second_item.value_;
  value_ %= N;

  return *this;
}

template <size_t N>
Residue<N> operator+(const Residue<N>& first_item, const Residue<N>& second_item) {
  Residue<N> temp_item(first_item);
  temp_item += second_item;

  return temp_item;
}

template <size_t N>
Residue<N> operator-(const Residue<N>& item) {
  return Residue<N>(N - item.value_); 
}

template <size_t N>
Residue<N>& Residue<N>::operator-=(const Residue<N>& second_item) {
  return (*this += -second_item);
}

template <size_t N>
Residue<N> operator-(const Residue<N>& first_item, const Residue<N>& second_item) {
  Residue<N> temp_item(first_item);
  temp_item -= second_item;

  return temp_item;
}

template <size_t N>
Residue<N>& Residue<N>::operator*=(const Residue<N>& second_item) {
  value_ *= second_item.value_;
  value_ %= N;

  return *this;
}

template <size_t N>
Residue<N> operator*(const Residue<N>& first_item, const Residue<N>& second_item) {
  Residue<N> temp_item(first_item);
  temp_item *= second_item;

  return temp_item;
}

template <size_t N>
Residue<N>& Residue<N>::operator/=(const Residue<N>& second_item) {
  static_assert(isPrime::result);
  return (*this *= getReverseItem(second_item));
}

template <size_t N>
Residue<N> operator/(const Residue<N>& first_item, const Residue<N>& second_item) {
  Residue<N> temp_item = first_item;
  temp_item /= second_item;

  return temp_item;
}

template <size_t M, size_t N, typename Field = Rational>
class Matrix {
  private:
    std::vector<std::vector<Field>> matrix_;
    bool sign_ = false;

    void doStraightGaussianStroke(bool to_invert, Matrix<M, N, Field>&);
    void doReversalGaussianStroke(Matrix<M, N, Field>&);

  public:
    Matrix();
    Matrix(const std::vector<std::vector<Field>>& matrix) : matrix_(matrix) {}
    Matrix(const std::initializer_list<std::initializer_list<int>>& matrix); 


    Matrix<M, N, Field>& operator=(const Matrix<M, N, Field>&);
    Matrix<M, N, Field>& operator+=(const Matrix<M, N, Field>&);
    Matrix<M, N, Field>& operator-=(const Matrix<M, N, Field>&);

    Matrix<M, N, Field>& operator*=(const Matrix<M, N, Field>&);
    Matrix<M, N, Field>& operator*=(const Field&);
  
    std::vector<Field>& operator[](uint32_t row) { return matrix_[row]; }
    const std::vector<Field>& operator[](uint32_t row) const { return matrix_[row]; }

    Field det() const; 
    Field trace() const;
    uint32_t rank() const;
    
    Matrix<N, M, Field> transposed() const;
    Matrix<M, N, Field> inverted() const;
    Matrix<M, N, Field>& invert();

    std::vector<Field> getRow(uint32_t pos) const { return matrix_[pos]; }; 
    std::vector<Field> getColumn(uint32_t) const;
};

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix() : matrix_(M, std::vector<Field>(N, Field(0))) {
  if (M == N) {
    for (uint32_t row = 0; row < M; ++row) {
      matrix_[row][row] = Field(1);
    }
  }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix(const std::initializer_list<std::initializer_list<int>>& matrix) 
  : matrix_(M, std::vector<Field>(N)) {
  uint32_t row = 0;
  for (auto& item_list: matrix) {
    uint32_t column = 0;
    for (auto& item: item_list) {
      matrix_[row][column] = Field(item);
      ++column;
    }
    ++row;
  }  
};

template <size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::doStraightGaussianStroke(bool to_invert,
    Matrix<M, N, Field>& identity_matrix) { 
  uint32_t count_of_null_columns = 0;
  for (uint32_t fix_column = 0; fix_column < N; ++fix_column) {
    uint32_t fix_row = fix_column - count_of_null_columns;
    while (fix_row < M && matrix_[fix_row][fix_column] == Field(0)) {
      ++fix_row;
    }
    if (fix_row != M) {
      Field divider = matrix_[fix_row][fix_column];
      for (uint32_t row = fix_row + 1; row < M; ++row) {
        if (matrix_[row][fix_column] != Field(0)) {
          Field factor = matrix_[row][fix_column];
          for (uint32_t column = 0; column < N; ++column) {
              matrix_[row][column] -= matrix_[fix_row][column] * (factor / divider);
            if (to_invert) {
              identity_matrix[row][column] -= 
                identity_matrix[fix_row][column] * (factor / divider);
            }
          }
        }
      }
      if (fix_row != fix_column) {
        sign_ = !sign_;
        std::swap(matrix_[fix_column - count_of_null_columns], matrix_[fix_row]);
        if(to_invert) {
          std::swap(identity_matrix[fix_column - count_of_null_columns], 
              identity_matrix[fix_row]);
        }
      }
    } else {
      ++count_of_null_columns;
    }
  }
}

template <size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::doReversalGaussianStroke(
    Matrix<M, N, Field>& identity_matrix) {
  uint32_t fix_column = N;
  do {
    --fix_column;
    for (uint32_t column = 0; column < N; ++column) {
      identity_matrix[fix_column][column] /= matrix_[fix_column][fix_column];
    }
    matrix_[fix_column][fix_column] = Field(1);
    for (uint32_t row = 0; row < fix_column; ++row) {
      Field factor = matrix_[row][fix_column];
      matrix_[row][fix_column] = Field(0);
      for (uint32_t column = 0; column < N; ++column) {
        identity_matrix[row][column] -= identity_matrix[fix_column][column] * factor;
      }
    }
  } while (fix_column > 0);
}

template <size_t N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;


template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator=(
    const Matrix<M, N, Field>& second_matrix) {
  matrix_ = second_matrix.matrix_;
  sign_ = second_matrix.sign_;

  return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator+=(
    const Matrix<M, N, Field>& second_matrix) {
  for (uint32_t row = 0; row < M; ++row) {
    for (uint32_t column = 0; column < N; ++column) {
      matrix_[row][column] += second_matrix[row][column];
    }
  }

  return *this;
}

template <size_t M, size_t N, typename Field = Rational>
Matrix<M, N, Field> operator+(const Matrix<M, N, Field>& first_matrix, 
    const Matrix<M, N, Field>& second_matrix) {
  Matrix<M, N, Field> temp_matrix = first_matrix;
  temp_matrix += second_matrix;

  return temp_matrix;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator-=(
    const Matrix<M, N, Field>& second_matrix) {
  for (uint32_t row = 0; row < M; ++row) {
    for (uint32_t column = 0; column < N; ++column) {
      matrix_[row][column] -= second_matrix[row][column];
    }
  }

  return *this;
}

template <size_t M, size_t N, typename Field = Rational>
Matrix<M, N, Field> operator-(const Matrix<M, N, Field>& first_matrix, 
    const Matrix<M, N, Field>& second_matrix) {
  Matrix<M, N, Field> temp_matrix = first_matrix;
  temp_matrix -= second_matrix;

  return temp_matrix;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Matrix<M, N, Field>& second_matrix) {
  static_assert(M == N);
  *this = *this * second_matrix;

  return *this;
}

template <size_t M, size_t N, size_t K, typename Field = Rational>
Matrix<M, K, Field> operator*(const Matrix<M, N, Field>& first_matrix,
    const Matrix<N, K, Field>& second_matrix) {
  Matrix<M, K, Field> temp_matrix;
  for (uint32_t row = 0; row < M; ++row) {
    for (uint32_t column = 0; column < K; ++column) {
      temp_matrix[row][column] = Field(0);
      for (uint32_t ordinal = 0; ordinal < N; ++ordinal) {
        temp_matrix[row][column] += first_matrix[row][ordinal] * 
          second_matrix[ordinal][column];
      }
    }
  }

  return temp_matrix;
}

template <size_t M, size_t N, typename Field = Rational>
Matrix<M, N, Field> operator*(const Field& value, const Matrix<M, N, Field>& matrix) {
  Matrix<M, N, Field> temp_matrix(matrix);
  temp_matrix *= value;

  return temp_matrix;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Field& value) {
  for (uint32_t row = 0; row < M; ++row) {
    for (uint32_t column = 0; column < N; ++column) {
      matrix_[row][column] *= value;
    }
  }

  return *this;
}


template <size_t M, size_t N, typename Field = Rational>
bool operator==(const Matrix<M, N, Field>& first_matrix, 
    const Matrix<M, N, Field>& second_matrix) {
  for (uint32_t row = 0; row < M; ++row) {
    for (uint32_t column = 0; column < N; ++column) {
      if (first_matrix[row][column] != second_matrix[row][column]) {
        return false;
      }
    }
  }

  return true;
}

template <size_t M, size_t N, typename Field = Rational>
bool operator!=(const Matrix<M, N, Field>& first_matrix, 
    const Matrix<M, N, Field>& second_matrix) {
  return !(first_matrix == second_matrix);
}

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::det() const {
  static_assert(M == N);
  
  Matrix<M, N, Field> temp_matrix = *this, useless_matrix;
  temp_matrix.doStraightGaussianStroke(false, useless_matrix);

  Field result = temp_matrix.sign_ ? -1 : 1;
  for (uint32_t row = 0; row < M; ++row) {
    result *= temp_matrix[row][row];
  } 

  return result;
}

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::trace() const {
  static_assert(M == N);

  Field result = 0; 
  for (uint32_t row = 0; row < M; ++row) {
    result += matrix_[row][row];
  }
  
  return result;
}

template <size_t M, size_t N, typename Field>
uint32_t Matrix<M, N, Field>::rank() const {
  Matrix<M, N, Field> temp_matrix = *this, useless_matrix;
  temp_matrix.doStraightGaussianStroke(false, useless_matrix);
  uint32_t result = 0;
  for (uint32_t row = 0; row < M; ++row) {
    if (temp_matrix[row] != std::vector<Field>(N, Field(0))) {
      ++result;
    }
  }

  return result;
}

template <size_t M, size_t N, typename Field>
std::vector<Field> Matrix<M, N, Field>::getColumn(uint32_t pos) const {
  std::vector<Field> result(M);
  for (uint32_t ordinal = 0; ordinal < M; ++ordinal) {
    result[ordinal] = matrix_[ordinal][pos];
  } 
  
  return result;
}

template <size_t M, size_t N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
  Matrix<N, M, Field> result;
  for (uint32_t row = 0; row < N; ++row) {
    for (uint32_t column = 0; column < M; ++column) {
      result[row][column] = matrix_[column][row];
    }
  }
  
  return result;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
  Matrix<M, N, Field> temp_matrix = *this;
  
  return temp_matrix.invert();
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::invert() {
  static_assert(M == N);
  Matrix<M, N, Field> result;

  doStraightGaussianStroke(true, result);
  doReversalGaussianStroke(result);

  return (*this = result); 
}
