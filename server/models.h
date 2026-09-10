#ifndef MODELS_H
#define MODELS_H

// =====================================================
// models.h - Cac lop mo hinh du lieu + danh sach lien ket (template)
// Dung chung cho ban web cua do an PBL2 Quan Ly Khach San.
// Chi khai bao + ham inline ngan; phan xu ly nghiep vu nam o store.cpp
// =====================================================

#include <string>
#include <sstream>
#include <vector>
#include <functional>

using std::string;

// -----------------------------------------------------
// Ngay thang (dd/MM/yyyy) - rut gon tu ban console cu
// -----------------------------------------------------
struct Ngay {
    int ngay = 1, thang = 1, nam = 2026;

    Ngay() {}
    Ngay(int d, int m, int y) : ngay(d), thang(m), nam(y) {}

    static bool namNhuan(int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    bool hopLe() const {
        if (nam < 1900 || nam > 2100) return false;
        if (thang < 1 || thang > 12) return false;
        static const int sn[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int max = sn[thang - 1];
        if (thang == 2 && namNhuan(nam)) max = 29;
        return ngay >= 1 && ngay <= max;
    }

    // Doi ra so thu tu ngay de tru/so sanh
    long soThuTu() const {
        static const int sn[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        long t = (long)nam * 365 + nam / 4 - nam / 100 + nam / 400;
        for (int i = 0; i < thang - 1; i++) t += sn[i];
        if (thang > 2 && namNhuan(nam)) t++;
        return t + ngay;
    }

    string toString() const {
        std::ostringstream o;
        o << (ngay < 10 ? "0" : "") << ngay << "/"
          << (thang < 10 ? "0" : "") << thang << "/" << nam;
        return o.str();
    }

    // Nhan chuoi "dd/MM/yyyy". Tra ve false neu sai dinh dang / ngay khong hop le.
    static bool parse(const string& s, Ngay& out) {
        int d = 0, m = 0, y = 0;
        char c1 = 0, c2 = 0;
        std::istringstream is(s);
        is >> d >> c1 >> m >> c2 >> y;
        if (is.fail() || c1 != '/' || c2 != '/') return false;
        Ngay tmp(d, m, y);
        if (!tmp.hopLe()) return false;
        out = tmp;
        return true;
    }

    bool operator<(const Ngay& o) const { return soThuTu() < o.soThuTu(); }
    bool operator<=(const Ngay& o) const { return soThuTu() <= o.soThuTu(); }
};

// So dem giua 2 ngay, toi thieu 1 (o qua dem tinh 1 dem)
inline int soDem(const Ngay& nhan, const Ngay& tra) {
    long d = tra.soThuTu() - nhan.soThuTu();
    if (d < 1) d = 1;
    return (int)d;
}

// -----------------------------------------------------
// Danh sach lien ket don - template, dung lai cho moi loai ban ghi
// (day la yeu cau cot loi cua PBL2: tu cai dat cau truc du lieu)
// -----------------------------------------------------
template <class T>
class DanhSachLK {
public:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    DanhSachLK() : head_(nullptr), duoi_(nullptr), n_(0) {}
    ~DanhSachLK() { xoaHet(); }

    DanhSachLK(const DanhSachLK&) = delete;
    DanhSachLK& operator=(const DanhSachLK&) = delete;

    Node* head() const { return head_; }
    int size() const { return n_; }
    bool rong() const { return head_ == nullptr; }

    void xoaHet() {
        Node* c = head_;
        while (c) { Node* t = c; c = c->next; delete t; }
        head_ = duoi_ = nullptr;
        n_ = 0;
    }

    // Them vao cuoi (giu thu tu chen)
    void themCuoi(const T& d) {
        Node* nn = new Node(d);
        if (!head_) head_ = duoi_ = nn;
        else { duoi_->next = nn; duoi_ = nn; }
        n_++;
    }

    // Duyet toan bo, goi fn(data) cho tung phan tu
    void moi(const std::function<void(T&)>& fn) {
        for (Node* c = head_; c; c = c->next) fn(c->data);
    }
    void moi(const std::function<void(const T&)>& fn) const {
        for (Node* c = head_; c; c = c->next) fn(c->data);
    }

    // Tra ve con tro toi phan tu dau tien thoa dieu kien, hoac nullptr
    T* tim(const std::function<bool(const T&)>& dk) {
        for (Node* c = head_; c; c = c->next)
            if (dk(c->data)) return &c->data;
        return nullptr;
    }

    // Xoa phan tu dau tien thoa dieu kien. Tra ve true neu co xoa.
    bool xoaMot(const std::function<bool(const T&)>& dk) {
        Node* truoc = nullptr;
        for (Node* c = head_; c; truoc = c, c = c->next) {
            if (!dk(c->data)) continue;
            if (truoc) truoc->next = c->next; else head_ = c->next;
            if (c == duoi_) duoi_ = truoc;
            delete c;
            n_--;
            return true;
        }
        return false;
    }

    // Dem so phan tu thoa dieu kien
    int dem(const std::function<bool(const T&)>& dk) const {
        int k = 0;
        for (Node* c = head_; c; c = c->next) if (dk(c->data)) k++;
        return k;
    }

private:
    Node* head_;
    Node* duoi_;
    int n_;
};

// -----------------------------------------------------
// Cac ban ghi nghiep vu
// -----------------------------------------------------

// Trang thai phong
namespace TrangThaiPhong {
    const string TRONG   = "TRONG";
    const string DANG_O  = "DANG_O";
    const string BAO_TRI = "BAO_TRI";
}

// Trang thai luot dat phong
namespace TrangThaiDat {
    const string DA_DAT = "DA_DAT";   // da dat, chua nhan phong
    const string DANG_O = "DANG_O";   // da check-in
    const string DA_TRA = "DA_TRA";   // da check-out
    const string DA_HUY = "DA_HUY";
}

// Nhom dich vu
namespace NhomDV {
    const string THUC_AN   = "THUC_AN";
    const string NUOC_UONG = "NUOC_UONG";
    const string SPA       = "SPA";
    const string KHAC      = "KHAC";
}

struct Phong {
    string maPhong;      // khoa chinh, vd "A203"
    string loaiPhong;    // "A" / "B" / "C" ... (nhan tu nguoi dung)
    long long donGia = 0;// gia mot dem (VND)
    int sucChua = 2;
    string trangThai = TrangThaiPhong::TRONG;
    string moTa;
};

struct DichVu {
    string maDV;         // khoa chinh, vd "DV01"
    string tenDV;        // "Buffet sang", "Nuoc suoi", "Massage body"
    string nhom = NhomDV::KHAC;
    long long donGia = 0;// gia ban
    long long giaVon = 0;// gia von (de tinh loi nhuan)
    string donViTinh = "lan";
};

struct DatPhong {
    string maDat;        // khoa chinh, vd "BK001"
    string tenKhach;
    string cccd;
    string sdt;
    string maPhong;      // FK -> Phong
    Ngay ngayNhan;
    Ngay ngayTra;
    long long tienCoc = 0;
    string trangThai = TrangThaiDat::DA_DAT;
    string ngayTao;      // ISO "yyyy-MM-dd HH:mm:ss" - chi de tham chieu
};

struct SuDungDV {
    string maSD;         // khoa chinh, vd "SD0001"
    string maDat;        // FK -> DatPhong
    string maDV;         // FK -> DichVu (tham chieu)
    string tenDV;        // snapshot ten luc su dung
    long long donGia = 0;// snapshot gia ban luc su dung
    long long giaVon = 0;// snapshot gia von luc su dung
    int soLuong = 1;
    string ngay;         // "dd/MM/yyyy"
};

#endif // MODELS_H
