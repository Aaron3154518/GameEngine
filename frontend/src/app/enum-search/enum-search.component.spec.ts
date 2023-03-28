import { ComponentFixture, TestBed } from '@angular/core/testing';

import { EnumSearchComponent } from './enum-search.component';

describe('EnumSearchComponent', () => {
  let component: EnumSearchComponent;
  let fixture: ComponentFixture<EnumSearchComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ EnumSearchComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(EnumSearchComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
