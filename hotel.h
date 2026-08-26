#ifndef HOTEL_H
#define HOTEL_H

#include <iostream>
#include <string>

using namespace std;

// =====================================================
// class Date - luu 1 ngay (ngay/thang/nam)
// =====================================================
class Date {
public:
    int day, month, year;

    Date(int d = 1, int m = 1, int y = 2026) : day(d), month(m), year(y) {}

    bool isValid() const;
    long toDayNumber() const;      // doi ngay thanh 1 so nguyen de so sanh/tru
    string toString() const;       // dang "dd/mm/yyyy" de in ra man hinh

    bool operator<(const Date& other) const;
};

bool isLeapYear(int year);   // ham phu, dung chung cho Date

// =====================================================
// class KhachHang - 1 hoa don / 1 khach o khach san
// =====================================================
class KhachHang {
public:
    // ---- Du lieu goc: doc/ghi truc tiep tu file ----
    string maHD;        // khoa chinh, khong duoc trung
    string tenKhach;
    string cccd;
    string gioiTinh;     // "Nam" / "Nu" / "Khac"
    string soPhong;      // vd "A203"
    Date ngayDen;
    Date ngayDi;

    // ---- Du lieu suy ra: tinh lai tu du lieu goc, KHONG ghi xuong file ----
    char loaiPhong;
    int  donGia;
    int  soNgay;
    long tienPhong;

    KhachHang() : loaiPhong(' '), donGia(0), soNgay(0), tienPhong(0) {}

    void tinhToanLaiDuLieu();        // tinh lai loaiPhong, donGia, soNgay, tienPhong
    void hienThiDong(int stt) const; // in 1 dong trong bang danh sach
};

// =====================================================
// class Node - 1 nut cua danh sach lien ket don
// =====================================================
class Node {
public:
    KhachHang data;
    Node* next;

    Node(const KhachHang& kh) : data(kh), next(nullptr) {}
};

// =====================================================
// class DanhSachKhachSan - quan ly toan bo danh sach
// =====================================================
class DanhSachKhachSan {
private:
    Node* head;   // con tro dau danh sach - chi class nay duoc truy cap truc tiep

public:
    DanhSachKhachSan() : head(nullptr) {}
    ~DanhSachKhachSan() { freeList(); }   // tu dong giai phong khi doi tuong ket thuc vong doi

    void freeList();
    bool isEmpty() const { return head == nullptr; }
    int  demSoLuong() const;

    // ----- Them -----
    void addHead(const KhachHang& kh);
    void addTail(const KhachHang& kh);
    bool insertAtPosition(const KhachHang& kh, int pos);
    bool insertSorted(const KhachHang& kh, const string& tieuChi);

    // ----- Xoa -----
    bool deleteByInvoice(const string& maHD);
    bool deleteByCCCD(const string& cccd);
    int  deleteCustomersNamedMinh();

    // ----- Tim kiem -----
    void searchByInvoice(const string& maHD) const;
    void searchByName(const string& ten) const;
    void searchByCCCD(const string& cccd) const;
    void searchByRoom(const string& soPhong) const;
    void searchByRoomType(char loai) const;

    // ----- Kiem tra trung ma HD (dung khi them/chen) -----
    bool tonTaiMaHD(const string& maHD) const;

    // ----- Sap xep -----
    void sortBy(const string& tieuChi, bool tangDan);

    // ----- Thong ke -----
    void statisticsByRoomType() const;

    // ----- Hien thi -----
    void display() const;

    // ----- File -----
    bool readFile(const string& filename);
    bool writeFile(const string& filename) const;
};

// =====================================================
// Ham validate du lieu nhap
// =====================================================
bool validateMaHD(const string& s);
bool validateCCCD(const string& s);
bool validateSoPhong(const string& s);
bool validateGioiTinh(const string& s);

#endif
