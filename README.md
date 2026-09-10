# Quản lý khách sạn — PBL2 (phiên bản website)

Ứng dụng quản lý khách sạn gồm **backend C++** (REST API, dùng thư viện single-header
[cpp-httplib](https://github.com/yhirose/cpp-httplib)) và **giao diện web tĩnh** HTML/CSS/JS
thuần. Dữ liệu lưu trong các **file văn bản** ở thư mục `data/` (mỗi dòng một bản ghi,
các trường ngăn cách bằng ký tự `|`) — bám sát cách làm của bản console gốc.

## Chức năng

| Mục | Mô tả |
|-----|-------|
| **Danh sách phòng** | Thêm / sửa / xóa phòng, đơn giá theo đêm, sức chứa, trạng thái (Trống / Đang ở / Bảo trì). |
| **Đặt phòng & Check-in/out** | Tạo lượt đặt kèm **tiền cọc**, check-in, check-out sinh **hóa đơn tổng** (tiền phòng + dịch vụ − cọc = còn phải trả), hủy lượt đặt. |
| **Dịch vụ bổ sung** | Danh mục dịch vụ (thức ăn, nước uống, spa/massage, khác) có đơn giá & giá vốn; thêm dịch vụ vào từng lượt đặt. |
| **Thống kê lợi nhuận** | Lọc theo khoảng ngày trả phòng; doanh thu/lợi nhuận phòng & dịch vụ, tách theo loại phòng và nhóm dịch vụ. |

## Yêu cầu

- `g++` hỗ trợ C++17 (khuyến nghị **MSYS2 / MinGW-w64**), có trong `PATH`.
- Windows (đã kèm `-lws2_32`). Trên Linux/macOS xem mục dưới.

## Biên dịch & chạy

### Windows
```bat
build.bat            :: tạo hotelweb.exe
run.bat              :: build (nếu cần) + mở trình duyệt + chạy server cổng 8080
hotelweb.exe 9000    :: chạy ở cổng khác
```

### Dùng make (mọi nền tảng có MinGW)
```sh
mingw32-make        # hoặc: make
./hotelweb.exe
```

### Linux / macOS (tùy chọn)
```sh
g++ -std=c++17 -O2 -pthread server/main.cpp server/store.cpp -o hotelweb
./hotelweb
```

Sau khi chạy, mở trình duyệt tại **http://localhost:8080**.

## Cấu trúc thư mục

```
server/
  models.h      Ngày, danh sách liên kết template DanhSachLK<T>, các bản ghi
  json.h        tiện ích xuất JSON tối giản
  store.h/.cpp  lớp Store: nghiệp vụ + đọc/ghi 4 file trong data/
  main.cpp      HTTP server + định tuyến REST + phục vụ thư mục web/
web/
  index.html, style.css, app.js     giao diện một trang, 4 tab
data/
  rooms.txt            maPhong|loai|donGia|sucChua|trangThai|moTa
  services.txt         maDV|tenDV|nhom|donGia|giaVon|donViTinh
  bookings.txt         maDat|tenKhach|cccd|sdt|maPhong|ngayNhan|ngayTra|tienCoc|trangThai|ngayTao
  service_orders.txt   maSD|maDat|maDV|tenDV|donGia|giaVon|soLuong|ngay
third_party/httplib.h  thư viện HTTP (MIT, không sửa)
```

Bản console gốc (`main.cpp`, `hotel.cpp`, `hotel.h`, `data.txt` ở thư mục gốc) **được giữ nguyên**.

## REST API (tóm tắt)

| Method & path | Ý nghĩa |
|---|---|
| `GET /api/rooms` · `POST /api/rooms` · `POST /api/rooms/{ma}/update` · `POST /api/rooms/{ma}/delete` | CRUD phòng |
| `GET/POST /api/services` · `POST /api/services/{ma}/update` · `.../delete` | CRUD dịch vụ |
| `GET /api/bookings` (`?trangThai=`) · `POST /api/bookings` | Danh sách / tạo lượt đặt |
| `POST /api/bookings/{ma}/checkin` · `/checkout` · `/cancel` | Đổi trạng thái lượt đặt (checkout trả về hóa đơn) |
| `GET /api/bookings/{ma}/bill` | Hóa đơn hiện thời |
| `GET/POST /api/bookings/{ma}/services` · `POST /api/service-orders/{ma}/delete` | Dịch vụ của lượt đặt |
| `GET /api/stats` (`?tu=dd/MM/yyyy&den=dd/MM/yyyy`) | Thống kê lợi nhuận |

Dữ liệu gửi lên dạng `application/x-www-form-urlencoded`; phản hồi là JSON.
