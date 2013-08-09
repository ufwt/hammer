#include "hammer.hxx"

namespace hammer {

  typedef variant<BytesResult, UintResult, IntResult, NullResult, SequenceResult> AnyResult;

  const BytesResult::result_type BytesResult::result() { return _bytes; }
  const UintResult::result_type UintResult::result() { return _uint; }
  const IntResult::result_type IntResult::result() { return _sint; }
  const SequenceResult::result_type SequenceResult::result() { return _seq; }

  template<>
  BytesResult Parser<BytesResult>::parse(const string &input) {
    HParseResult *res = h_parse(_parser, reinterpret_cast<const uint8_t*>(input.c_str()), input.size());
    return BytesResult(vector<uint8_t>(res->ast->bytes.token, res->ast->bytes.token+res->ast->bytes.len));
  }

  template<>
  BytesResult Parser<BytesResult>::parse(const uint8_t *input, size_t length) {
    HParseResult *res = h_parse(_parser, input, length);
    return BytesResult(vector<uint8_t>(res->ast->bytes.token, res->ast->bytes.token+res->ast->bytes.len));
  }

  template<>
  UintResult Parser<UintResult>::parse(const string &input) {
    HParseResult *res = h_parse(_parser, reinterpret_cast<const uint8_t*>(input.c_str()), input.size());
    return UintResult(res->ast->uint);
  }

  template<>
  UintResult Parser<UintResult>::parse(const uint8_t *input, size_t length) {
    HParseResult *res = h_parse(_parser, input, length);
    return UintResult(res->ast->uint);
  }

  template<>
  IntResult Parser<IntResult>::parse(const string &input) {
    HParseResult *res = h_parse(_parser, reinterpret_cast<const uint8_t*>(input.c_str()), input.size());
    return IntResult(res->ast->sint);
  }

  template<>
  IntResult Parser<IntResult>::parse(const uint8_t *input, size_t length) {
    HParseResult *res = h_parse(_parser, input, length);
    return IntResult(res->ast->sint);
  }

  template<>
  NullResult Parser<NullResult>::parse(const string &input) {
    HParseResult *res = h_parse(_parser, reinterpret_cast<const uint8_t*>(input.c_str()), input.size());
    return NullResult();
  }

  template<>
  NullResult Parser<NullResult>::parse(const uint8_t *input, size_t length) {
    HParseResult *res = h_parse(_parser, input, length);
    return NullResult();
  }

  vector<AnyResult> make_seq(HCountedArray *seq) {
    vector<AnyResult> ret;
    for (size_t i=0; i<seq->used; ++i) {
      switch(seq->elements[i]->token_type) {
      case TT_NONE:
	ret.push_back(NullResult());
	break;
      case TT_BYTES:
	ret.push_back(BytesResult(vector<uint8_t>(seq->elements[i]->bytes.token, seq->elements[i]->bytes.token+seq->elements[i]->bytes.len)));
	break;
      case TT_SINT:
	ret.push_back(IntResult(seq->elements[i]->sint));
	break;
      case TT_UINT:
	ret.push_back(UintResult(seq->elements[i]->uint));
	break;
      case TT_SEQUENCE:
	ret.push_back(make_seq(seq->elements[i]->seq));
	break;
      default:
	//TODO some kind of error
	break;
      }
    }
    return ret;
  }

  template<>
  SequenceResult Parser<SequenceResult>::parse(const string &input) {
    HParseResult *res = h_parse(_parser, reinterpret_cast<const uint8_t*>(input.c_str()), input.size());
    return SequenceResult(make_seq(res->ast->seq));
  }

  template<>
  SequenceResult Parser<SequenceResult>::parse(const uint8_t *input, size_t length) {
    HParseResult *res = h_parse(_parser, input, length);
    return SequenceResult(make_seq(res->ast->seq));
  }

  template<class T>
  Many<T> Parser<T>::many() {
    return Many<T>(this);
  }

  template<class T>
  RepeatN<T> Parser<T>::many(size_t n) {
    return RepeatN<T>(this, n);
  }

  template<class T>
  Optional<T> Parser<T>::optional() {
    return Optional<T>(this);
  }

  template<class T>
  RepeatN<T> Parser<T>::operator[](size_t n) {
    return RepeatN<T>(this, n);
  }

  /*
  Int64 Int64::in_range(const int64_t lower, const int64_t upper) {
    return IntRange(Int64(), lower, upper);
  }

  Int32 Int32::in_range(const int32_t lower, const int32_t upper) {
    return IntRange(Int32(), lower, upper);
  }

  Int16 Int16::in_range(const int16_t lower, const int16_t upper) {
    return IntRange(Int16(), lower, upper);
  }

  Int8 Int8::in_range(const int8_t lower, const int8_t upper) {
    return IntRange(Int8(), lower, upper);
  }

  Uint64 Uint64::in_range(const uint64_t lower, const uint64_t upper) {
    return IntRange(Uint64(), lower, upper);
  }

  Uint32 Uint32::in_range(const uint32_t lower, const uint32_t upper) {
    return IntRange(Uint32(), lower, upper);
  }

  Uint16 Uint16::in_range(const uint16_t lower, const uint16_t upper) {
    return IntRange(Uint16(), lower, upper);
  }

  Uint8 Uint8::in_range(const uint8_t lower, const uint8_t upper) {
    return IntRange(Uint8(), lower, upper);
  }
  */
}
