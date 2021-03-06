import { Component, OnInit } from '@angular/core';
import { Router, ActivatedRoute } from '@angular/router';
import { combineLatest, BehaviorSubject } from 'rxjs';
import {
  map,
  tap,
  share,
  shareReplay,
  distinctUntilChanged,
  debounceTime,
} from 'rxjs/operators';

import { LocalAppService } from '../../services/local-app.service';
import { App } from 'app/services/app.service';
import { AuthService } from 'app/services/auth.service';

@Component({
  selector: 'dstore-local-app',
  templateUrl: './local-app.component.html',
  styleUrls: ['./local-app.component.scss'],
})
export class LocalAppComponent implements OnInit {
  constructor(
    private route: ActivatedRoute,
    private router: Router,
    private localAppService: LocalAppService,
    private authService: AuthService,
  ) {}
  DisabledList = DisabledList;
  logged = this.authService.logged$;
  removing: string[] = [];
  select: string;
  listHeight$ = new BehaviorSubject<number>(0);
  pageSize$ = this.listHeight$.pipe(
    debounceTime(100),
    map(height => Math.floor(height / 64)),
    distinctUntilChanged(),
  );
  pageIndex$ = this.route.queryParamMap.pipe(
    map(query => Number(query.get('page') || 1) - 1),
  );
  localApps$ = this.localAppService.LocalAppList().pipe(share());
  apps$ = combineLatest(this.localApps$, this.pageSize$, this.pageIndex$).pipe(
    map(([apps, size, index]) => {
      return apps.slice(size * index, size * (index + 1));
    }),
  );
  count$ = combineLatest(this.localApps$, this.pageSize$).pipe(
    map(([apps, size]) => {
      return Math.ceil(apps.length / size);
    }),
  );
  removingList$ = this.localAppService.RemovingList();

  gotoPage(pageIndex: number) {
    this.router.navigate([], { queryParams: { page: pageIndex + 1 } });
  }

  remove(app: App) {
    this.select = null;
    this.removing.push(app.name);
    this.localAppService.RemoveLocalApp(app);
  }

  ngOnInit() {}
}

const DisabledList = [
  'dde-control-center',
  'dde-introduction',
  'dde-file-manager',
  'deepin-appstore',
  'deepin-manual',
];
