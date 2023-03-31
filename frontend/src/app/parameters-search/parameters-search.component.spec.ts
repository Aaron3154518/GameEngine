import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ParametersSearchComponent } from './parameters-search.component';

describe('ParametersSearchComponent', () => {
  let component: ParametersSearchComponent;
  let fixture: ComponentFixture<ParametersSearchComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ParametersSearchComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ParametersSearchComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
