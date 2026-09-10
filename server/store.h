#ifndef STORE_H
#define STORE_H

// =====================================================
// store.h - Lop Store: giu 4 danh sach lien ket + toan bo nghiep vu
// (them/sua/xoa, check-in/check-out, dat dich vu, tinh hoa don, thong ke)
// va doc/ghi ra cac file van ban trong thu muc data/.
// =====================================================

#include "models.h"
#include <string>

// Ket qua chung cua mot thao tac nghiep vu
struct KetQua {
    bool ok = false;
    string thongBao;      // mo ta loi/thanh cong (tieng Viet khong dau)
    string jsonData;      // du lieu tra ve (raw JSON), rong neu khong co

    static KetQua loi(const string& m) { return KetQua{false, m, ""}; }
    static KetQua thanhCong(const string& json = "null") { return KetQua{true, "", json}; }
};

class Store {
public:
    explicit Store(const string& thuMucData);

    void napTatCa();      // doc 4 file
    void luuTatCa() const;

    // ---------- Phong ----------
    string dsPhongJson() const;                       // kem thong tin luot dat hien tai
    KetQua themPhong(const string& maPhong, const string& loai, long long donGia,
                     int sucChua, const string& moTa);
    KetQua suaPhong(const string& maPhong, const string& loai, long long donGia,
                    int sucChua, const string& trangThai, const string& moTa);
    KetQua xoaPhong(const string& maPhong);

    // ---------- Dich vu ----------
    string dsDichVuJson() const;
    KetQua themDichVu(const string& ten, const string& nhom, long long donGia,
                      long long giaVon, const string& dvt);
    KetQua suaDichVu(const string& maDV, const string& ten, const string& nhom,
                     long long donGia, long long giaVon, const string& dvt);
    KetQua xoaDichVu(const string& maDV);

    // ---------- Dat phong ----------
    string dsDatPhongJson(const string& locTrangThai = "") const;
    KetQua taoDatPhong(const string& tenKhach, const string& cccd, const string& sdt,
                       const string& maPhong, const string& ngayNhan, const string& ngayTra,
                       long long tienCoc);
    KetQua checkIn(const string& maDat);
    KetQua checkOut(const string& maDat);
    KetQua huyDat(const string& maDat);
    string hoaDonJson(const string& maDat) const;     // {ok, ...} hoac {ok:false}

    // ---------- Su dung dich vu ----------
    string dsSuDungJson(const string& maDat) const;
    KetQua themSuDung(const string& maDat, const string& maDV, int soLuong, const string& ngay);
    KetQua xoaSuDung(const string& maSD);

    // ---------- Thong ke ----------
    // tu / den: "dd/MM/yyyy" (rong = khong gioi han). Loc theo ngay check-out.
    string thongKeJson(const string& tu, const string& den) const;

private:
    string dir_;
    DanhSachLK<Phong> phong_;
    DanhSachLK<DichVu> dichVu_;
    DanhSachLK<DatPhong> datPhong_;
    DanhSachLK<SuDungDV> suDung_;

    // helpers
    string duongDan(const string& ten) const { return dir_ + "/" + ten; }
    const Phong* timPhong(const string& ma) const;
    Phong* timPhong(const string& ma);
    const DichVu* timDichVu(const string& ma) const;
    const DatPhong* timDat(const string& ma) const;
    DatPhong* timDat(const string& ma);

    long long tienDichVuCuaDat(const string& maDat) const;
    long long tienVonDichVuCuaDat(const string& maDat) const;
    string hoaDonJsonNoiBo(const DatPhong& d) const;

    string sinhMa(const string& tienTo, int soChuSo) const;

    // load/save tung file
    void napPhong();
    void napDichVu();
    void napDatPhong();
    void napSuDung();
    void luuPhong() const;
    void luuDichVu() const;
    void luuDatPhong() const;
    void luuSuDung() const;
};

#endif // STORE_H
