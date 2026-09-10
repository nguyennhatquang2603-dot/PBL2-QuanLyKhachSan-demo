#include "store.h"
#include "json.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <cstdlib>

// =====================================================
// Tien ich noi bo
// =====================================================
namespace {

std::vector<string> tach(const string& line, char sep = '|') {
    std::vector<string> r;
    string cur;
    for (char c : line) {
        if (c == sep) { r.push_back(cur); cur.clear(); }
        else cur += c;
    }
    r.push_back(cur);
    return r;
}

string cat(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

long long toLL(const string& s) {
    try { return std::stoll(cat(s)); } catch (...) { return 0; }
}
int toInt(const string& s) {
    try { return std::stoi(cat(s)); } catch (...) { return 0; }
}

std::vector<string> docDong(const string& path) {
    std::vector<string> out;
    std::ifstream f(path.c_str());
    if (!f.is_open()) return out;
    string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (cat(line).empty()) continue;
        out.push_back(line);
    }
    return out;
}

string bayGioISO() {
    std::time_t t = std::time(nullptr);
    std::tm lt{};
#if defined(_WIN32)
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &lt);
    return buf;
}

string homNay() {
    std::time_t t = std::time(nullptr);
    std::tm lt{};
#if defined(_WIN32)
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    char buf[16];
    std::strftime(buf, sizeof(buf), "%d/%m/%Y", &lt);
    return buf;
}

bool laNhomHopLe(const string& n) {
    return n == NhomDV::THUC_AN || n == NhomDV::NUOC_UONG ||
           n == NhomDV::SPA || n == NhomDV::KHAC;
}

} // namespace

// =====================================================
// Khoi tao
// =====================================================
Store::Store(const string& thuMucData) : dir_(thuMucData) {}

void Store::napTatCa() {
    napPhong();
    napDichVu();
    napDatPhong();
    napSuDung();
}

void Store::luuTatCa() const {
    luuPhong();
    luuDichVu();
    luuDatPhong();
    luuSuDung();
}

// =====================================================
// Doc file
// Dinh dang mot dong = mot ban ghi, cac truong cach nhau bang '|'
// =====================================================
void Store::napPhong() {
    phong_.xoaHet();
    // maPhong|loaiPhong|donGia|sucChua|trangThai|moTa
    for (auto& l : docDong(duongDan("rooms.txt"))) {
        auto f = tach(l);
        if (f.size() < 5) continue;
        Phong p;
        p.maPhong   = cat(f[0]);
        p.loaiPhong = cat(f[1]);
        p.donGia    = toLL(f[2]);
        p.sucChua   = toInt(f[3]);
        p.trangThai = cat(f[4]);
        p.moTa      = f.size() > 5 ? cat(f[5]) : "";
        if (p.trangThai != TrangThaiPhong::TRONG &&
            p.trangThai != TrangThaiPhong::DANG_O &&
            p.trangThai != TrangThaiPhong::BAO_TRI)
            p.trangThai = TrangThaiPhong::TRONG;
        phong_.themCuoi(p);
    }
}

void Store::napDichVu() {
    dichVu_.xoaHet();
    // maDV|tenDV|nhom|donGia|giaVon|donViTinh
    for (auto& l : docDong(duongDan("services.txt"))) {
        auto f = tach(l);
        if (f.size() < 5) continue;
        DichVu d;
        d.maDV      = cat(f[0]);
        d.tenDV     = cat(f[1]);
        d.nhom      = cat(f[2]);
        d.donGia    = toLL(f[3]);
        d.giaVon    = toLL(f[4]);
        d.donViTinh = f.size() > 5 ? cat(f[5]) : "lan";
        if (!laNhomHopLe(d.nhom)) d.nhom = NhomDV::KHAC;
        dichVu_.themCuoi(d);
    }
}

void Store::napDatPhong() {
    datPhong_.xoaHet();
    // maDat|tenKhach|cccd|sdt|maPhong|ngayNhan|ngayTra|tienCoc|trangThai|ngayTao
    for (auto& l : docDong(duongDan("bookings.txt"))) {
        auto f = tach(l);
        if (f.size() < 9) continue;
        DatPhong d;
        d.maDat     = cat(f[0]);
        d.tenKhach  = cat(f[1]);
        d.cccd      = cat(f[2]);
        d.sdt       = cat(f[3]);
        d.maPhong   = cat(f[4]);
        Ngay::parse(cat(f[5]), d.ngayNhan);
        Ngay::parse(cat(f[6]), d.ngayTra);
        d.tienCoc   = toLL(f[7]);
        d.trangThai = cat(f[8]);
        d.ngayTao   = f.size() > 9 ? cat(f[9]) : "";
        datPhong_.themCuoi(d);
    }
}

void Store::napSuDung() {
    suDung_.xoaHet();
    // maSD|maDat|maDV|tenDV|donGia|giaVon|soLuong|ngay
    for (auto& l : docDong(duongDan("service_orders.txt"))) {
        auto f = tach(l);
        if (f.size() < 8) continue;
        SuDungDV s;
        s.maSD    = cat(f[0]);
        s.maDat   = cat(f[1]);
        s.maDV    = cat(f[2]);
        s.tenDV   = cat(f[3]);
        s.donGia  = toLL(f[4]);
        s.giaVon  = toLL(f[5]);
        s.soLuong = toInt(f[6]);
        s.ngay    = cat(f[7]);
        suDung_.themCuoi(s);
    }
}

// =====================================================
// Ghi file
// =====================================================
void Store::luuPhong() const {
    std::ofstream f(duongDan("rooms.txt").c_str());
    phong_.moi([&](const Phong& p) {
        f << p.maPhong << "|" << p.loaiPhong << "|" << p.donGia << "|"
          << p.sucChua << "|" << p.trangThai << "|" << p.moTa << "\n";
    });
}

void Store::luuDichVu() const {
    std::ofstream f(duongDan("services.txt").c_str());
    dichVu_.moi([&](const DichVu& d) {
        f << d.maDV << "|" << d.tenDV << "|" << d.nhom << "|" << d.donGia << "|"
          << d.giaVon << "|" << d.donViTinh << "\n";
    });
}

void Store::luuDatPhong() const {
    std::ofstream f(duongDan("bookings.txt").c_str());
    datPhong_.moi([&](const DatPhong& d) {
        f << d.maDat << "|" << d.tenKhach << "|" << d.cccd << "|" << d.sdt << "|"
          << d.maPhong << "|" << d.ngayNhan.toString() << "|" << d.ngayTra.toString()
          << "|" << d.tienCoc << "|" << d.trangThai << "|" << d.ngayTao << "\n";
    });
}

void Store::luuSuDung() const {
    std::ofstream f(duongDan("service_orders.txt").c_str());
    suDung_.moi([&](const SuDungDV& s) {
        f << s.maSD << "|" << s.maDat << "|" << s.maDV << "|" << s.tenDV << "|"
          << s.donGia << "|" << s.giaVon << "|" << s.soLuong << "|" << s.ngay << "\n";
    });
}

// =====================================================
// Helpers tra cuu
// =====================================================
const Phong* Store::timPhong(const string& ma) const {
    return const_cast<Store*>(this)->timPhong(ma);
}
Phong* Store::timPhong(const string& ma) {
    return phong_.tim([&](const Phong& p) { return p.maPhong == ma; });
}
const DichVu* Store::timDichVu(const string& ma) const {
    return const_cast<DanhSachLK<DichVu>&>(dichVu_).tim(
        [&](const DichVu& d) { return d.maDV == ma; });
}
const DatPhong* Store::timDat(const string& ma) const {
    return const_cast<Store*>(this)->timDat(ma);
}
DatPhong* Store::timDat(const string& ma) {
    return datPhong_.tim([&](const DatPhong& d) { return d.maDat == ma; });
}

string Store::sinhMa(const string& tienTo, int soChuSo) const {
    int maxSo = 0;
    auto quet = [&](const string& ma) {
        if (ma.rfind(tienTo, 0) != 0) return;
        int v = std::atoi(ma.substr(tienTo.size()).c_str());
        if (v > maxSo) maxSo = v;
    };
    if (tienTo == "P")  phong_.moi([&](const Phong& p) { quet(p.maPhong); });
    if (tienTo == "DV") dichVu_.moi([&](const DichVu& d) { quet(d.maDV); });
    if (tienTo == "BK") datPhong_.moi([&](const DatPhong& d) { quet(d.maDat); });
    if (tienTo == "SD") suDung_.moi([&](const SuDungDV& s) { quet(s.maSD); });

    std::ostringstream o;
    o << tienTo;
    string num = std::to_string(maxSo + 1);
    for (int i = (int)num.size(); i < soChuSo; i++) o << "0";
    o << num;
    return o.str();
}

// =====================================================
// PHONG
// =====================================================
string Store::dsPhongJson() const {
    std::ostringstream o;
    o << "[";
    bool first = true;
    phong_.moi([&](const Phong& p) {
        if (!first) o << ",";
        first = false;

        // luot dat dang hoat dong cua phong nay (DA_DAT hoac DANG_O)
        const DatPhong* dHt = const_cast<DanhSachLK<DatPhong>&>(datPhong_).tim(
            [&](const DatPhong& d) {
                return d.maPhong == p.maPhong &&
                       (d.trangThai == TrangThaiDat::DA_DAT ||
                        d.trangThai == TrangThaiDat::DANG_O);
            });

        js::Obj ob;
        ob.ks("maPhong", p.maPhong)
          .ks("loaiPhong", p.loaiPhong)
          .kv("donGia", p.donGia)
          .kv("sucChua", p.sucChua)
          .ks("trangThai", p.trangThai)
          .ks("moTa", p.moTa);
        if (dHt) {
            js::Obj lt;
            lt.ks("maDat", dHt->maDat)
              .ks("tenKhach", dHt->tenKhach)
              .ks("ngayNhan", dHt->ngayNhan.toString())
              .ks("ngayTra", dHt->ngayTra.toString())
              .ks("trangThai", dHt->trangThai);
            ob.kraw("luotDat", lt.str());
        } else {
            ob.kraw("luotDat", "null");
        }
        o << ob.str();
    });
    o << "]";
    return o.str();
}

KetQua Store::themPhong(const string& maPhong, const string& loai, long long donGia,
                        int sucChua, const string& moTa) {
    string ma = cat(maPhong);
    if (ma.empty()) return KetQua::loi("Ma phong khong duoc rong");
    if (timPhong(ma)) return KetQua::loi("Ma phong da ton tai");
    if (donGia < 0) return KetQua::loi("Don gia khong hop le");

    Phong p;
    p.maPhong = ma;
    p.loaiPhong = cat(loai).empty() ? string(1, (char)std::toupper(ma[0])) : cat(loai);
    p.donGia = donGia;
    p.sucChua = sucChua > 0 ? sucChua : 2;
    p.trangThai = TrangThaiPhong::TRONG;
    p.moTa = cat(moTa);
    phong_.themCuoi(p);
    luuPhong();
    return KetQua::thanhCong();
}

KetQua Store::suaPhong(const string& maPhong, const string& loai, long long donGia,
                       int sucChua, const string& trangThai, const string& moTa) {
    Phong* p = timPhong(cat(maPhong));
    if (!p) return KetQua::loi("Khong tim thay phong");

    string tt = cat(trangThai);
    if (tt == TrangThaiPhong::BAO_TRI || tt == TrangThaiPhong::TRONG) {
        // khong cho chuyen sang BAO_TRI/TRONG neu dang co khach o
        const DatPhong* dO = const_cast<DanhSachLK<DatPhong>&>(datPhong_).tim(
            [&](const DatPhong& d) {
                return d.maPhong == p->maPhong && d.trangThai == TrangThaiDat::DANG_O;
            });
        if (dO) return KetQua::loi("Phong dang co khach, khong the doi trang thai");
        p->trangThai = tt;
    } else if (tt == TrangThaiPhong::DANG_O) {
        return KetQua::loi("Trang thai DANG_O duoc dat tu dong khi check-in");
    }

    if (!cat(loai).empty()) p->loaiPhong = cat(loai);
    if (donGia >= 0) p->donGia = donGia;
    if (sucChua > 0) p->sucChua = sucChua;
    p->moTa = cat(moTa);
    luuPhong();
    return KetQua::thanhCong();
}

KetQua Store::xoaPhong(const string& maPhong) {
    const Phong* p = timPhong(cat(maPhong));
    if (!p) return KetQua::loi("Khong tim thay phong");

    const DatPhong* dang = const_cast<DanhSachLK<DatPhong>&>(datPhong_).tim(
        [&](const DatPhong& d) {
            return d.maPhong == p->maPhong &&
                   (d.trangThai == TrangThaiDat::DA_DAT || d.trangThai == TrangThaiDat::DANG_O);
        });
    if (dang) return KetQua::loi("Phong con luot dat/khach dang o, khong the xoa");

    phong_.xoaMot([&](const Phong& x) { return x.maPhong == cat(maPhong); });
    luuPhong();
    return KetQua::thanhCong();
}

// =====================================================
// DICH VU
// =====================================================
string Store::dsDichVuJson() const {
    std::ostringstream o;
    o << "[";
    bool first = true;
    dichVu_.moi([&](const DichVu& d) {
        if (!first) o << ",";
        first = false;
        js::Obj ob;
        ob.ks("maDV", d.maDV)
          .ks("tenDV", d.tenDV)
          .ks("nhom", d.nhom)
          .kv("donGia", d.donGia)
          .kv("giaVon", d.giaVon)
          .ks("donViTinh", d.donViTinh);
        o << ob.str();
    });
    o << "]";
    return o.str();
}

KetQua Store::themDichVu(const string& ten, const string& nhom, long long donGia,
                         long long giaVon, const string& dvt) {
    if (cat(ten).empty()) return KetQua::loi("Ten dich vu khong duoc rong");
    if (donGia < 0 || giaVon < 0) return KetQua::loi("Gia khong hop le");

    DichVu d;
    d.maDV = sinhMa("DV", 2);
    d.tenDV = cat(ten);
    d.nhom = laNhomHopLe(cat(nhom)) ? cat(nhom) : NhomDV::KHAC;
    d.donGia = donGia;
    d.giaVon = giaVon;
    d.donViTinh = cat(dvt).empty() ? "lan" : cat(dvt);
    dichVu_.themCuoi(d);
    luuDichVu();
    return KetQua::thanhCong();
}

KetQua Store::suaDichVu(const string& maDV, const string& ten, const string& nhom,
                        long long donGia, long long giaVon, const string& dvt) {
    DichVu* d = dichVu_.tim([&](const DichVu& x) { return x.maDV == cat(maDV); });
    if (!d) return KetQua::loi("Khong tim thay dich vu");
    if (!cat(ten).empty()) d->tenDV = cat(ten);
    if (laNhomHopLe(cat(nhom))) d->nhom = cat(nhom);
    if (donGia >= 0) d->donGia = donGia;
    if (giaVon >= 0) d->giaVon = giaVon;
    if (!cat(dvt).empty()) d->donViTinh = cat(dvt);
    luuDichVu();
    return KetQua::thanhCong();
}

KetQua Store::xoaDichVu(const string& maDV) {
    if (!dichVu_.tim([&](const DichVu& x) { return x.maDV == cat(maDV); }))
        return KetQua::loi("Khong tim thay dich vu");
    int dangDung = suDung_.dem([&](const SuDungDV& s) { return s.maDV == cat(maDV); });
    if (dangDung > 0)
        return KetQua::loi("Dich vu da phat sinh su dung, khong the xoa (co the sua gia)");
    dichVu_.xoaMot([&](const DichVu& x) { return x.maDV == cat(maDV); });
    luuDichVu();
    return KetQua::thanhCong();
}

// =====================================================
// DAT PHONG
// =====================================================
static string datPhongJson(const DatPhong& d, const string& tenPhongLoai,
                           long long donGiaPhong) {
    js::Obj ob;
    ob.ks("maDat", d.maDat)
      .ks("tenKhach", d.tenKhach)
      .ks("cccd", d.cccd)
      .ks("sdt", d.sdt)
      .ks("maPhong", d.maPhong)
      .ks("loaiPhong", tenPhongLoai)
      .kv("donGiaPhong", donGiaPhong)
      .ks("ngayNhan", d.ngayNhan.toString())
      .ks("ngayTra", d.ngayTra.toString())
      .kv("soDem", soDem(d.ngayNhan, d.ngayTra))
      .kv("tienCoc", d.tienCoc)
      .ks("trangThai", d.trangThai)
      .ks("ngayTao", d.ngayTao);
    return ob.str();
}

string Store::dsDatPhongJson(const string& locTrangThai) const {
    std::ostringstream o;
    o << "[";
    bool first = true;
    const_cast<DanhSachLK<DatPhong>&>(datPhong_).moi([&](const DatPhong& d) {
        if (!locTrangThai.empty() && d.trangThai != locTrangThai) return;
        if (!first) o << ",";
        first = false;
        const Phong* p = timPhong(d.maPhong);
        o << datPhongJson(d, p ? p->loaiPhong : "", p ? p->donGia : 0);
    });
    o << "]";
    return o.str();
}

KetQua Store::taoDatPhong(const string& tenKhach, const string& cccd, const string& sdt,
                          const string& maPhong, const string& ngayNhan, const string& ngayTra,
                          long long tienCoc) {
    if (cat(tenKhach).empty()) return KetQua::loi("Ten khach khong duoc rong");
    Phong* p = timPhong(cat(maPhong));
    if (!p) return KetQua::loi("Khong tim thay phong");
    if (p->trangThai == TrangThaiPhong::BAO_TRI)
        return KetQua::loi("Phong dang bao tri");

    Ngay nn, nt;
    if (!Ngay::parse(cat(ngayNhan), nn)) return KetQua::loi("Ngay nhan phong khong hop le");
    if (!Ngay::parse(cat(ngayTra), nt)) return KetQua::loi("Ngay tra phong khong hop le");
    if (!(nn < nt)) return KetQua::loi("Ngay tra phai sau ngay nhan");
    if (tienCoc < 0) return KetQua::loi("Tien coc khong hop le");

    // phong chi cho 1 luot dat/o dang hoat dong
    const DatPhong* dang = const_cast<DanhSachLK<DatPhong>&>(datPhong_).tim(
        [&](const DatPhong& x) {
            return x.maPhong == p->maPhong &&
                   (x.trangThai == TrangThaiDat::DA_DAT || x.trangThai == TrangThaiDat::DANG_O);
        });
    if (dang) return KetQua::loi("Phong da co luot dat/khach dang hoat dong");

    DatPhong d;
    d.maDat = sinhMa("BK", 3);
    d.tenKhach = cat(tenKhach);
    d.cccd = cat(cccd);
    d.sdt = cat(sdt);
    d.maPhong = p->maPhong;
    d.ngayNhan = nn;
    d.ngayTra = nt;
    d.tienCoc = tienCoc;
    d.trangThai = TrangThaiDat::DA_DAT;
    d.ngayTao = bayGioISO();
    datPhong_.themCuoi(d);
    luuDatPhong();

    js::Obj ob; ob.ks("maDat", d.maDat);
    return KetQua::thanhCong(ob.str());
}

KetQua Store::checkIn(const string& maDat) {
    DatPhong* d = timDat(cat(maDat));
    if (!d) return KetQua::loi("Khong tim thay luot dat");
    if (d->trangThai != TrangThaiDat::DA_DAT)
        return KetQua::loi("Chi co the check-in luot dang o trang thai DA_DAT");
    Phong* p = timPhong(d->maPhong);
    if (!p) return KetQua::loi("Khong tim thay phong cua luot dat");

    d->trangThai = TrangThaiDat::DANG_O;
    p->trangThai = TrangThaiPhong::DANG_O;
    luuDatPhong();
    luuPhong();
    return KetQua::thanhCong();
}

KetQua Store::checkOut(const string& maDat) {
    DatPhong* d = timDat(cat(maDat));
    if (!d) return KetQua::loi("Khong tim thay luot dat");
    if (d->trangThai != TrangThaiDat::DANG_O)
        return KetQua::loi("Chi co the check-out luot dang o trang thai DANG_O");
    Phong* p = timPhong(d->maPhong);

    string hd = hoaDonJsonNoiBo(*d);   // chot hoa don truoc khi doi trang thai
    d->trangThai = TrangThaiDat::DA_TRA;
    if (p) p->trangThai = TrangThaiPhong::TRONG;
    luuDatPhong();
    luuPhong();
    return KetQua::thanhCong(hd);
}

KetQua Store::huyDat(const string& maDat) {
    DatPhong* d = timDat(cat(maDat));
    if (!d) return KetQua::loi("Khong tim thay luot dat");
    if (d->trangThai == TrangThaiDat::DA_TRA)
        return KetQua::loi("Luot dat da tra phong, khong the huy");
    if (d->trangThai == TrangThaiDat::DA_HUY)
        return KetQua::loi("Luot dat da bi huy truoc do");

    Phong* p = timPhong(d->maPhong);
    d->trangThai = TrangThaiDat::DA_HUY;
    if (p && p->trangThai == TrangThaiPhong::DANG_O)
        p->trangThai = TrangThaiPhong::TRONG;
    luuDatPhong();
    luuPhong();
    return KetQua::thanhCong();
}

long long Store::tienDichVuCuaDat(const string& maDat) const {
    long long t = 0;
    const_cast<DanhSachLK<SuDungDV>&>(suDung_).moi([&](const SuDungDV& s) {
        if (s.maDat == maDat) t += s.donGia * s.soLuong;
    });
    return t;
}
long long Store::tienVonDichVuCuaDat(const string& maDat) const {
    long long t = 0;
    const_cast<DanhSachLK<SuDungDV>&>(suDung_).moi([&](const SuDungDV& s) {
        if (s.maDat == maDat) t += s.giaVon * s.soLuong;
    });
    return t;
}

string Store::hoaDonJsonNoiBo(const DatPhong& d) const {
    const Phong* p = timPhong(d.maPhong);
    long long donGiaPhong = p ? p->donGia : 0;
    int dem = soDem(d.ngayNhan, d.ngayTra);
    long long tienPhong = donGiaPhong * dem;
    long long tienDV = tienDichVuCuaDat(d.maDat);
    long long tongCong = tienPhong + tienDV;
    long long conLai = tongCong - d.tienCoc;

    // danh sach dong dich vu
    std::ostringstream arr;
    arr << "[";
    bool first = true;
    const_cast<DanhSachLK<SuDungDV>&>(suDung_).moi([&](const SuDungDV& s) {
        if (s.maDat != d.maDat) return;
        if (!first) arr << ",";
        first = false;
        js::Obj ob;
        ob.ks("maSD", s.maSD)
          .ks("tenDV", s.tenDV)
          .kv("donGia", s.donGia)
          .kv("soLuong", s.soLuong)
          .kv("thanhTien", s.donGia * s.soLuong)
          .ks("ngay", s.ngay);
        arr << ob.str();
    });
    arr << "]";

    js::Obj ob;
    ob.kv("ok", "true")
      .ks("maDat", d.maDat)
      .ks("tenKhach", d.tenKhach)
      .ks("maPhong", d.maPhong)
      .ks("loaiPhong", p ? p->loaiPhong : "")
      .ks("ngayNhan", d.ngayNhan.toString())
      .ks("ngayTra", d.ngayTra.toString())
      .kv("soDem", dem)
      .kv("donGiaPhong", donGiaPhong)
      .kv("tienPhong", tienPhong)
      .kraw("dichVu", arr.str())
      .kv("tienDichVu", tienDV)
      .kv("tienCoc", d.tienCoc)
      .kv("tongCong", tongCong)
      .kv("conLai", conLai)
      .ks("trangThai", d.trangThai);
    return ob.str();
}

string Store::hoaDonJson(const string& maDat) const {
    const DatPhong* d = timDat(cat(maDat));
    if (!d) return "{\"ok\":false,\"thongBao\":\"Khong tim thay luot dat\"}";
    return hoaDonJsonNoiBo(*d);
}

// =====================================================
// SU DUNG DICH VU
// =====================================================
string Store::dsSuDungJson(const string& maDat) const {
    std::ostringstream o;
    o << "[";
    bool first = true;
    const_cast<DanhSachLK<SuDungDV>&>(suDung_).moi([&](const SuDungDV& s) {
        if (s.maDat != cat(maDat)) return;
        if (!first) o << ",";
        first = false;
        js::Obj ob;
        ob.ks("maSD", s.maSD)
          .ks("maDV", s.maDV)
          .ks("tenDV", s.tenDV)
          .kv("donGia", s.donGia)
          .kv("soLuong", s.soLuong)
          .kv("thanhTien", s.donGia * s.soLuong)
          .ks("ngay", s.ngay);
        o << ob.str();
    });
    o << "]";
    return o.str();
}

KetQua Store::themSuDung(const string& maDat, const string& maDV, int soLuong,
                         const string& ngay) {
    DatPhong* d = timDat(cat(maDat));
    if (!d) return KetQua::loi("Khong tim thay luot dat");
    if (d->trangThai != TrangThaiDat::DANG_O && d->trangThai != TrangThaiDat::DA_DAT)
        return KetQua::loi("Chi them dich vu cho luot dang o hoac da dat");
    const DichVu* dv = timDichVu(cat(maDV));
    if (!dv) return KetQua::loi("Khong tim thay dich vu");
    if (soLuong <= 0) return KetQua::loi("So luong phai lon hon 0");

    SuDungDV s;
    s.maSD = sinhMa("SD", 4);
    s.maDat = d->maDat;
    s.maDV = dv->maDV;
    s.tenDV = dv->tenDV;
    s.donGia = dv->donGia;
    s.giaVon = dv->giaVon;
    s.soLuong = soLuong;
    s.ngay = cat(ngay).empty() ? homNay() : cat(ngay);
    suDung_.themCuoi(s);
    luuSuDung();
    return KetQua::thanhCong();
}

KetQua Store::xoaSuDung(const string& maSD) {
    if (!suDung_.tim([&](const SuDungDV& s) { return s.maSD == cat(maSD); }))
        return KetQua::loi("Khong tim thay dong dich vu");
    // khong cho xoa neu luot dat da tra phong (hoa don da chot)
    SuDungDV* s = suDung_.tim([&](const SuDungDV& x) { return x.maSD == cat(maSD); });
    const DatPhong* d = timDat(s->maDat);
    if (d && d->trangThai == TrangThaiDat::DA_TRA)
        return KetQua::loi("Luot dat da tra phong, hoa don da chot");
    suDung_.xoaMot([&](const SuDungDV& x) { return x.maSD == cat(maSD); });
    luuSuDung();
    return KetQua::thanhCong();
}

// =====================================================
// THONG KE LOI NHUAN
// Chi tinh cac luot da CHECK-OUT (DA_TRA). Loc theo ngay tra phong.
// loiNhuan = doanhThuPhong + (doanhThuDichVu - giaVonDichVu)
// =====================================================
string Store::thongKeJson(const string& tu, const string& den) const {
    Ngay nTu, nDen;
    bool coTu = Ngay::parse(cat(tu), nTu);
    bool coDen = Ngay::parse(cat(den), nDen);

    long long dtPhong = 0, dtDV = 0, vonDV = 0, tongCoc = 0;
    int soLuot = 0, tongDem = 0;

    // breakdown theo loai phong
    struct RT { long long doanhThu = 0; int luot = 0; int dem = 0; };
    std::vector<std::pair<string, RT>> theoLoai;
    auto themLoai = [&](const string& loai, long long tien, int dem) {
        for (auto& e : theoLoai)
            if (e.first == loai) { e.second.doanhThu += tien; e.second.luot++; e.second.dem += dem; return; }
        RT r; r.doanhThu = tien; r.luot = 1; r.dem = dem;
        theoLoai.push_back({loai, r});
    };

    // breakdown theo nhom dich vu
    struct SG { long long doanhThu = 0; long long von = 0; int soLuong = 0; };
    std::vector<std::pair<string, SG>> theoNhom;
    auto themNhom = [&](const string& nhom, long long dt, long long v, int sl) {
        for (auto& e : theoNhom)
            if (e.first == nhom) { e.second.doanhThu += dt; e.second.von += v; e.second.soLuong += sl; return; }
        SG g; g.doanhThu = dt; g.von = v; g.soLuong = sl;
        theoNhom.push_back({nhom, g});
    };

    const_cast<DanhSachLK<DatPhong>&>(datPhong_).moi([&](const DatPhong& d) {
        if (d.trangThai != TrangThaiDat::DA_TRA) return;
        if (coTu && d.ngayTra < nTu) return;
        if (coDen && nDen < d.ngayTra) return;

        const Phong* p = timPhong(d.maPhong);
        long long donGiaPhong = p ? p->donGia : 0;
        int dem = soDem(d.ngayNhan, d.ngayTra);
        long long tienPhong = donGiaPhong * dem;

        long long tienDV = tienDichVuCuaDat(d.maDat);
        long long vDV = tienVonDichVuCuaDat(d.maDat);

        dtPhong += tienPhong;
        dtDV += tienDV;
        vonDV += vDV;
        tongCoc += d.tienCoc;
        soLuot++;
        tongDem += dem;
        themLoai(p ? p->loaiPhong : "?", tienPhong, dem);

        const_cast<DanhSachLK<SuDungDV>&>(suDung_).moi([&](const SuDungDV& s) {
            if (s.maDat != d.maDat) return;
            const DichVu* dv = timDichVu(s.maDV);
            string nhom = dv ? dv->nhom : NhomDV::KHAC;
            themNhom(nhom, s.donGia * s.soLuong, s.giaVon * s.soLuong, s.soLuong);
        });
    });

    long long loiNhuanDV = dtDV - vonDV;
    long long tongLoiNhuan = dtPhong + loiNhuanDV;
    long long tongDoanhThu = dtPhong + dtDV;

    std::ostringstream loaiArr;
    loaiArr << "[";
    for (size_t i = 0; i < theoLoai.size(); i++) {
        if (i) loaiArr << ",";
        js::Obj ob;
        ob.ks("loaiPhong", theoLoai[i].first)
          .kv("doanhThu", theoLoai[i].second.doanhThu)
          .kv("soLuot", theoLoai[i].second.luot)
          .kv("soDem", theoLoai[i].second.dem);
        loaiArr << ob.str();
    }
    loaiArr << "]";

    std::ostringstream nhomArr;
    nhomArr << "[";
    for (size_t i = 0; i < theoNhom.size(); i++) {
        if (i) nhomArr << ",";
        js::Obj ob;
        ob.ks("nhom", theoNhom[i].first)
          .kv("doanhThu", theoNhom[i].second.doanhThu)
          .kv("giaVon", theoNhom[i].second.von)
          .kv("loiNhuan", theoNhom[i].second.doanhThu - theoNhom[i].second.von)
          .kv("soLuong", theoNhom[i].second.soLuong);
        nhomArr << ob.str();
    }
    nhomArr << "]";

    // cong suat: tong so phong hien co
    int soPhong = phong_.size();

    js::Obj ob;
    ob.kv("ok", "true")
      .ks("tu", coTu ? nTu.toString() : "")
      .ks("den", coDen ? nDen.toString() : "")
      .kv("soLuotKhach", soLuot)
      .kv("tongDem", tongDem)
      .kv("soPhong", soPhong)
      .kv("doanhThuPhong", dtPhong)
      .kv("doanhThuDichVu", dtDV)
      .kv("giaVonDichVu", vonDV)
      .kv("loiNhuanDichVu", loiNhuanDV)
      .kv("tongDoanhThu", tongDoanhThu)
      .kv("tongLoiNhuan", tongLoiNhuan)
      .kv("tongTienCoc", tongCoc)
      .kraw("theoLoaiPhong", loaiArr.str())
      .kraw("theoNhomDichVu", nhomArr.str());
    return ob.str();
}
