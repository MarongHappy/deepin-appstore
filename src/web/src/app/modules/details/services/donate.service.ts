import { Injectable } from '@angular/core';
import { Payment, PayReq, PayResp, PayCheck } from './donate.model';
import { BaseService } from 'app/dstore/services/base.service';
import { HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root',
})
export class DonateService {
  server = BaseService.serverHosts.metadataServer;
  constructor(private http: HttpClient) {}
  donate(payment: Payment, req: PayReq) {
    console.log('donate', payment, req);
    switch (payment) {
      case Payment.WeChat:
        return this.http.post<PayResp>(this.server + '/api/donate/wechatpay', req);
      case Payment.AliPay:
        return this.http.post<PayResp>(this.server + '/api/donate/alipay', req);
      case Payment.PayPal:
        return this.http.post<PayResp>(this.server + '/api/donate/paypal', req);
    }
  }
  check(payment: Payment, id: string) {
    if (payment === Payment.WeChat) {
      return this.http.get<PayCheck>(this.server + '/api/donate/wechatpay/check/' + id);
    }
    return this.http.get<PayCheck>(this.server + '/api/donate/check/' + id);
  }
}
