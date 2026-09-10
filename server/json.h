#ifndef JSON_H
#define JSON_H

// =====================================================
// json.h - Tien ich xuat JSON toi gian (khong dung thu vien ngoai).
// Chi phuc vu viec TRA VE du lieu cho frontend; du lieu GUI LEN tu
// frontend dung dang form-urlencoded nen khong can bo phan tich JSON.
// =====================================================

#include <string>
#include <sstream>

namespace js {

// Boc chuoi trong dau nhay kep + escape cac ky tu dac biet
inline std::string s(const std::string& v) {
    std::string o = "\"";
    for (char c : v) {
        switch (c) {
            case '"':  o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            case '\t': o += "\\t";  break;
            default:
                if ((unsigned char)c < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    o += buf;
                } else {
                    o += c;
                }
        }
    }
    o += "\"";
    return o;
}

// Bo dung xay object/array JSON bang cach noi chuoi, tu quan ly dau phay
class Obj {
public:
    Obj() { o_ << "{"; }
    template <class V>
    Obj& kv(const std::string& k, const V& v) {
        sep();
        o_ << js::s(k) << ":" << v;
        return *this;
    }
    Obj& ks(const std::string& k, const std::string& v) {
        sep();
        o_ << js::s(k) << ":" << js::s(v);
        return *this;
    }
    Obj& kraw(const std::string& k, const std::string& rawJson) {
        sep();
        o_ << js::s(k) << ":" << rawJson;
        return *this;
    }
    std::string str() { return o_.str() + "}"; }
private:
    void sep() { if (!first_) o_ << ","; first_ = false; }
    std::ostringstream o_;
    bool first_ = true;
};

} // namespace js

#endif // JSON_H
